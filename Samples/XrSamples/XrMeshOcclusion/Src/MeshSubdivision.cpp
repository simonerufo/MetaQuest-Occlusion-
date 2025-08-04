#include "MeshSubdivision.h"
#include <algorithm> // for std::min/max
#include <cmath>     // for cos

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void LoopSubdivision::expand_mesh(
    std::vector<XrVector3f>& vertices,
    const std::vector<uint32_t>& indices,
    float expansion_factor) {

    if (std::abs(expansion_factor) < 1e-6) {
        return; // No expansion needed
    }

    // Step 1: Calculate vertex normals by averaging face normals.
    // We use a vector of the same size as vertices to accumulate the normals.
    std::vector<XrVector3f> vertex_normals(vertices.size(), {0.0f, 0.0f, 0.0f});

    // Iterate over each triangle in the mesh
    for (size_t i = 0; i < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        const XrVector3f& v0 = vertices[i0];
        const XrVector3f& v1 = vertices[i1];
        const XrVector3f& v2 = vertices[i2];
        
        // Calculate the face normal using the cross product of two edges
        XrVector3f edge1 = VectorMath::subtract(v1, v0);
        XrVector3f edge2 = VectorMath::subtract(v2, v0);
        XrVector3f face_normal = VectorMath::cross_product(edge1, edge2);

        // Add this face normal to the accumulator for each vertex of the face.
        // We don't normalize the face normal here. Using the non-normalized
        // vector provides area weighting automatically (larger triangles contribute more).
        vertex_normals[i0] = VectorMath::add(vertex_normals[i0], face_normal);
        vertex_normals[i1] = VectorMath::add(vertex_normals[i1], face_normal);
        vertex_normals[i2] = VectorMath::add(vertex_normals[i2], face_normal);
    }

    // Step 2: Normalize the accumulated normals and expand the vertices.
    for (size_t i = 0; i < vertices.size(); ++i) {
        // Normalize the final accumulated normal vector
        XrVector3f normal = VectorMath::normalize(vertex_normals[i]);

        // Move the vertex position along its normal
        XrVector3f offset = VectorMath::scalar_multiply(normal, expansion_factor);
        vertices[i] = VectorMath::add(vertices[i], offset);
    }
}

std::pair<std::vector<XrVector3f>, std::vector<uint32_t>> LoopSubdivision::subdivide(
    const std::vector<XrVector3f>& originalVertices,
    const std::vector<uint32_t>& originalIndices,
    int iterations) {

    if (iterations <= 0) {
        return {originalVertices, originalIndices};
    }

    std::vector<XrVector3f> currentVertices = originalVertices;
    std::vector<uint32_t> currentIndices = originalIndices;

    for (int i = 0; i < iterations; ++i) {
        auto result = apply_subdivision(currentVertices, currentIndices);
        currentVertices = result.first;
        currentIndices = result.second;
    }

    return {currentVertices, currentIndices};
}


// Internal method to apply one level of subdivision
std::pair<std::vector<XrVector3f>, std::vector<uint32_t>> LoopSubdivision::apply_subdivision(
    const std::vector<XrVector3f>& vertices,
    const std::vector<uint32_t>& indices) {
    
    std::vector<XrVector3f> newVertices = vertices;
    std::vector<uint32_t> newIndices;
    
    // Map from an edge to the index of the new vertex created at its midpoint
    std::map<Edge, uint32_t> edgeToMidpointIndex;
    
    // Adjacency list to find neighboring vertices for updating original vertex positions
    std::vector<std::vector<uint32_t>> adjacency(vertices.size());
    // Map to find the "opposite" vertex for a given edge, needed for new vertex position rule
    std::map<Edge, std::vector<uint32_t>> edgeToOppositeVertices;


    // First pass: Build adjacency, find opposite vertices, and create new faces
    for (size_t i = 0; i < indices.size(); i += 3) {
        uint32_t v0_idx = indices[i];
        uint32_t v1_idx = indices[i + 1];
        uint32_t v2_idx = indices[i + 2];

        uint32_t tri_indices[3] = {v0_idx, v1_idx, v2_idx};
        uint32_t mid_indices[3];

        for (int j = 0; j < 3; ++j) {
            uint32_t p0_idx = tri_indices[j];
            uint32_t p1_idx = tri_indices[(j + 1) % 3];
            uint32_t opposite_idx = tri_indices[(j + 2) % 3];

            // Build adjacency list
            adjacency[p0_idx].push_back(p1_idx);
            adjacency[p1_idx].push_back(p0_idx);
            
            // Create a canonical representation for the edge
            Edge edge = std::make_pair(std::min(p0_idx, p1_idx), std::max(p0_idx, p1_idx));
            edgeToOppositeVertices[edge].push_back(opposite_idx);

            // If this edge hasn't been processed, create a new midpoint vertex
            if (edgeToMidpointIndex.find(edge) == edgeToMidpointIndex.end()) {
                edgeToMidpointIndex[edge] = newVertices.size();
                newVertices.push_back({0,0,0}); // Placeholder, position calculated later
            }
            mid_indices[j] = edgeToMidpointIndex[edge];
        }

        // Create the 4 new triangles from the original triangle
        newIndices.push_back(v0_idx); newIndices.push_back(mid_indices[0]); newIndices.push_back(mid_indices[2]);
        newIndices.push_back(v1_idx); newIndices.push_back(mid_indices[1]); newIndices.push_back(mid_indices[0]);
        newIndices.push_back(v2_idx); newIndices.push_back(mid_indices[2]); newIndices.push_back(mid_indices[1]);
        newIndices.push_back(mid_indices[0]); newIndices.push_back(mid_indices[1]); newIndices.push_back(mid_indices[2]);
    }
    
    // Second pass: Calculate the positions of the new edge vertices
    for (auto const& [edge, midpoint_idx] : edgeToMidpointIndex) {
        uint32_t v0_idx = edge.first;
        uint32_t v1_idx = edge.second;
        
        const XrVector3f& v0_pos = vertices[v0_idx];
        const XrVector3f& v1_pos = vertices[v1_idx];
        
        // Find the two opposite vertices (C and D in the formula)
        const auto& opposites = edgeToOppositeVertices[edge];
        if (opposites.size() == 2) { // Interior edge
            const XrVector3f& v2_pos = vertices[opposites[0]];
            const XrVector3f& v3_pos = vertices[opposites[1]];
            
            // New position = 3/8 * (v0 + v1) + 1/8 * (v2 + v3)
            auto term1 = VectorMath::scalar_multiply(VectorMath::add(v0_pos, v1_pos), 3.0f / 8.0f);
            auto term2 = VectorMath::scalar_multiply(VectorMath::add(v2_pos, v3_pos), 1.0f / 8.0f);
            newVertices[midpoint_idx] = VectorMath::add(term1, term2);
        } else { // Boundary edge
            // New position = 1/2 * (v0 + v1)
            newVertices[midpoint_idx] = VectorMath::scalar_multiply(VectorMath::add(v0_pos, v1_pos), 0.5f);
        }
    }
    
    // Third pass: Update the positions of the original vertices
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t k = adjacency[i].size();
        if (k < 2) continue; // Should not happen in a closed mesh

        // Beta calculation (Warren's formula, a common choice)
        float beta;
        if (k == 3) {
            beta = 3.0f / 16.0f;
        } else {
            beta = (3.0f / (8.0f * k));
        }
        
        // Sum of neighbor positions
        XrVector3f neighbor_sum = {0,0,0};
        for(uint32_t neighbor_idx : adjacency[i]) {
            neighbor_sum = VectorMath::add(neighbor_sum, vertices[neighbor_idx]);
        }
        
        // New position = (1 - k*beta) * old_pos + beta * neighbor_sum
        const XrVector3f& old_pos = vertices[i];
        auto term1 = VectorMath::scalar_multiply(old_pos, 1.0f - (float)k * beta);
        auto term2 = VectorMath::scalar_multiply(neighbor_sum, beta);
        newVertices[i] = VectorMath::add(term1, term2);
    }

    return {newVertices, newIndices};
}