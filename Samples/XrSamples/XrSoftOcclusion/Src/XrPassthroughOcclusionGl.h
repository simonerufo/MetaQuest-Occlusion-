/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <unordered_map>

#if defined(ANDROID)
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3ext.h>

#else
#include "Render/GlWrapperWin32.h"
#endif // defined(ANDROID)

#include "OVR_Math.h"

class Geometry {
   public:
    Geometry() = default;

    void CreateAxes();
    void CreatePlane();
    void CreateBox();
    void CreatePointCloudGrid(int gridWidth, int gridHeight);

    void Destroy();

    int GetIndexCount() const {
        return IndexCount;
    }
    GLuint GetVertexArrayObject() const {
        return VertexArrayObject;
    }
    int GetVertexCount() const {
        return VertexCount;
    }

   private:
    struct VertexAttribPointer {
        GLint Index = 0;
        GLint Size = 0;
        GLenum Type = 0;
        GLboolean Normalized = false;
        GLsizei Stride = 0;
        const GLvoid* Pointer = nullptr;
    };

    void CreateVAO();

    GLuint VertexArrayObject = 0;
    GLuint VertexBuffer = 0;
    GLuint IndexBuffer = 0;
    int VertexCount = 0;
    int IndexCount = 0;
    std::vector<VertexAttribPointer> VertexAttribs;
};

class Program {
   public:
    Program() = default;

    bool Create(const char* vertexSource, const char* fragmentSource);
    void Destroy();

    int GetProgramId() const {
        return Program_;
    }

    int GetUniformLocationOrDie(int uniformId) const;
    int GetUniformBindingOrDie(int uniformId) const;

   private:
    GLuint Program_ = 0;
    GLuint VertexShader = 0;
    GLuint FragmentShader = 0;

    // These will be -1 if not used by the program.
    std::unordered_map<int, GLint> UniformLocation;
    std::unordered_map<int, GLint> UniformBinding;
    std::unordered_map<int, GLint> Textures;
};

class Framebuffer {
   public:
    Framebuffer() = default;

    bool Create(
        GLenum colorFormat,
        int width,
        int height,
        int multisamples,
        int swapChainLength,
        GLuint* colorTextures);
    void Destroy();

    void Bind(int element);
    void Unbind();
    void Resolve();

    int GetWidth() const {
        return Width;
    }
    int GetHeight() const {
        return Height;
    }

   private:
    struct Element {
        GLuint ColorTexture = 0;
        GLuint DepthTexture = 0;
        GLuint FrameBufferObject = 0;
    };

    int Width = 0;
    int Height = 0;
    int Multisamples = 0;
    std::vector<Element> Elements;
};

class Scene {
   public:
    struct TrackedController {
        OVR::Posef Pose;
    };

    Scene() = default;

    void Create(int depthWidth, int depthHeight);
    void Destroy();

    bool IsCreated();
    void SetClearColor(const float* c);

    std::vector<TrackedController> TrackedControllers;

    GLuint SceneMatrices = 0;

    Program BoxDepthSpaceOcclusionProgram;
    Geometry Box;

    Program TemporalFilterProgram;
    GLuint FilteredDepthTextures[2] = {0};
    GLuint FilteredDepthFBOs[2] = {0};
    int HistoryBufferIndex = 0;
    int DepthWidth = 0;
    int DepthHeight = 0;
    void CreateTemporalFilterResources(int width, int height);

   private:
    bool CreatedScene = false;
};

class AppRenderer {
   public:
    // Occlusion culling
    struct BoundingSphere {
        OVR::Vector3f center;
        float radius;
    };
    
    struct OcclusionState {
        bool wasOccluded;
        int framesSinceVisible;
        int testFrequency;
        
        OcclusionState() : wasOccluded(false), framesSinceVisible(0), testFrequency(1) {}
    };

    std::vector<OcclusionState> controllerOcclusionStates;
    GLuint depthSamplerTexture = 0;
    
    int m_depthWidth = 0;
    int m_depthHeight = 0;    

    struct FrameIn {
        static constexpr int kNumEyes = 2;

        int SwapChainIndex = 0;
        OVR::Matrix4f View[kNumEyes];
        OVR::Matrix4f Proj[kNumEyes];

        bool HasDepth = false;

        // Render target metadata:
        float ScreenNearZ = 0.0f;
        float ScreenFarZ = 0.0f;

        // Depth texture metadata:
        GLuint DepthTexture = 0;
        float DepthNearZ = 0.0f;
        float DepthFarZ = 0.0f;

        // Depth space transform matrices:
        OVR::Matrix4f DepthViewMatrices[kNumEyes];
        OVR::Matrix4f DepthProjectionMatrices[kNumEyes];
    };

    AppRenderer() = default;
    

    void Create(
        GLenum format,
        int width,
        int height,
        int numMultiSamples,
        int swapChainLength,
        GLuint* colorTextures,
        int depthWidth,
        int depthHeight);

    void Destroy();

    void RenderFrame(const AppRenderer::FrameIn& frameIn);

    Scene scene;

   private:
    void RenderScene(const FrameIn& frameIn, GLuint filteredDepthTexture);
    GLuint RunTemporalFilterPass(GLuint rawDepthTexture);

    bool IsCreated = false;
    Framebuffer framebuffer;

    BoundingSphere calculateControllerBounds(const OVR::Matrix4f& modelMatrix);
    bool isBoundingSphereOccluded(const BoundingSphere& bounds, const AppRenderer::FrameIn& frameIn, int viewId);
    bool shouldTestOcclusion(OcclusionState& state);
    void updateOcclusionState(OcclusionState& state, bool currentlyOccluded);
    OVR::Vector2f getSampleOffset(int index, float radius);
    float sampleEnvironmentDepth(float x, float y, int viewId, GLuint depthTexture);
};
