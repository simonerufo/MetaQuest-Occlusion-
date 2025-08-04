#pragma once

#include <vector>
#include <map>
#include <utility>
#include "OVR_Math.h"
#include <openxr/openxr.h>

// Helper functions for XrVector3f math, as it doesn't have overloaded operators.
namespace VectorMath {
    inline XrVector3f add(const XrVector3f& a, const XrVector3f& b) {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    inline XrVector3f subtract(const XrVector3f& a, const XrVector3f& b) {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    inline XrVector3f scalar_multiply(const XrVector3f& v, float s) {
        return {v.x * s, v.y * s, v.z * s};
    }
    
    inline XrVector3f cross_product(const XrVector3f& a, const XrVector3f& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    inline float magnitude(const XrVector3f& v) {
        return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    inline XrVector3f normalize(const XrVector3f& v) {
        float mag = magnitude(v);
        if (mag > 1e-6) { // Avoid division by zero
            return scalar_multiply(v, 1.0f / mag);
        }
        return {0, 0, 0};
    }
}

class LoopSubdivision {
public:
    // Main function to call. It takes the original mesh data and the number of iterations.
    static std::pair<std::vector<XrVector3f>, std::vector<uint32_t>> subdivide(
        const std::vector<XrVector3f>& originalVertices,
        const std::vector<uint32_t>& originalIndices,
        int iterations);

    static void expand_mesh(
        std::vector<XrVector3f>& vertices,
        const std::vector<uint32_t>& indices,
        float expansion_factor);

private:
    // Represents an edge using the indices of its two endpoints. Sorted to be unique.
    using Edge = std::pair<uint32_t, uint32_t>;

    // Performs a single iteration of the subdivision algorithm.
    static std::pair<std::vector<XrVector3f>, std::vector<uint32_t>> apply_subdivision(
        const std::vector<XrVector3f>& vertices,
        const std::vector<uint32_t>& indices);
};