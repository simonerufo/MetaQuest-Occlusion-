# MetaQuest Occlusion Samples

## Table of contents  
- [Introduction](#introduction)  
- [Soft Occlusion](#socclusion)
- [Mesh Occlusion](#mocclusion)
- [How to build](#build)

## Introduction
This project is an educational resource for developers interested in Augmented Reality, focusing on the implementation of hologram occlusion with the Meta Quest.

The core technologies used are:

-   **[OpenXR API](https://registry.khronos.org/OpenXR/specs/1.0/styleguide.html):** For handling interactions with the Meta Quest hardware and its AR capabilities.
    
-   **[OpenGL ES 3.1](https://www.opengl.org/Documentation/Specs.html):** For all 3D rendering tasks, including geometry and GLSL shader programming.
    

The main objective of these samples is to provide a hands-on demonstration of how to achieve convincing occlusion of virtual content by the real world. Starting from the foundation provided by the [Meta OpenXR SDK](https://github.com/meta-quest/Meta-OpenXR-SDK), this repository explores and implements two different techniques for hologram occlusion.




## <a id="socclusion"></a> Soft Occlusion
![Alt Text](assets\SoftOcc.gif)

*Soft occlusion demo*

**OpenXR Extensions:** 
- XrPassthrough: This extension is what enables **Augmented Reality** on a typically **Virtual Reality** device. In simple terms, it gives developers control over the headset's built-in tracking cameras to display a live feed of the real world to the user.
- XrEnvironmentDepth: This is the "magic" that makes the interaction between the real and virtual worlds believable. While passthrough shows the real world, the environment depth functionality understands it in 3D. Specifically, this feature provides the application with a **depth map** of the user's surroundings.

**OpenGL shaders:** 
-  Vertex shader: a simple **model view projection matrix** definition.
- Fragment shader: The primary purpose of this shader is to draw a virtual object (a "hologram") in an Augmented Reality scene and make it realistically disappear behind real-world objects. To do this, it compares the depth of the virtual object against a depth map of the real world, which is provided by the Meta Quest depth sensor. The shader implements two techniques to make this occlusion look smooth and natural: **Soft Occlusion** and **Multi-Sampling**.

## <a id="mocclusion"></a> Mesh Occlusion

![Alt Text](assets\MeshOcc.gif)

*Mesh occlusion demo*

**OpenXR Extensions:**
- XrPassthrough
- XrFBScene:  Provides a simplified, high-level understanding of the room's geometry.
- XrFBSpatialEntity: This is the central pillar of the entire system. It establishes what a spatial anchor is and allows the runtime to track its position in the real world. Every other SPATIAL_ENTITY extension builds on this one.
- XrFBSpatialEntityQuery: Once the system knows about anchors (e.g., from a previously scanned room), this extension lets you find them. You can query based on location ("find all anchors within 2 meters of me") or by type ("find all anchors that are classified as 'table'").
-XrFBSpatialEntityStorage: This is what enables persistence in AR. When you place a virtual object on your real desk, you save the desk's spatial anchor. The next time the user starts your app, you can load that anchor, and the virtual object will appear in the exact same spot.
- XrFBSpatialEntityContainer: Allows spatial entities to contain other spatial entities, creating a hierarchy.
- XrFBSpatialEntityStorage: An optimization for saving or erasing multiple anchors at once.
- XrMetaSpatialEntityMesh: Provides the actual 3D triangle mesh for a spatial entity.
- XrMetaSpatialEntitySharing: This is the key to multi-user AR. By sharing anchors, you and a friend can see the same virtual object in the exact same real-world location. You share the anchor for your coffee table, and you can both see the virtual chessboard sitting on it.
- XrMetaSpatialEntityGroupSharing: A more advanced way to manage sharing with specific groups of users.
- XrFBSceneCapture: Provides an interface to initiate the process of scanning and "capturing" a new scene.


### How occlusion is achieved:
 The hologram occlusion here is achieved using a GPU technique known as Depth Buffering (or Z-Buffering):

 - The **"occluder"** pass:
  The first step is to inform the GPU about the geometry of the real world.
    the achievement is to create a "depth map" of the real world.
    The renderer draws the entire scene mesh provided by the headset (walls, floor, furniture).
    This pass is rendered with color writing turned OFF (glColorMask(false)). This means nothing is actually drawn to the screen, making the pass invisible. However, the GPU still populates the Depth Buffer with distance information for the real-world geometry.
    After this pass, the depth buffer contains a perfect, pixel-by-pixel "imprint" of the room's shape.

 - The **"visible scene"** pass:
  With the depth buffer primed, we can now draw our virtual content.
    we want to draw the virtual objects and have them be correctly hidden by the real world. The renderer draws the virtual objects (like the controller cubes) normally, with color writing turned back ON.
    For every single pixel of a virtual object, the GPU performs its standard, hardware-accelerated depth test. It asks a simple question: "Is this virtual pixel closer to the camera than the real-world pixel we recorded in Pass 1?"

      If yes, the pixel is drawn.

      If no, the pixel is hidden by the real world, so the GPU discards it automatically.

    This entire process happens for every frame, ensuring that the occlusion is always up-to-date with the user's position.


**Mesh refinement:**
- **Loop Subdivision**: This algorithm is called to increase the triangle density of the scene mesh. It makes the mesh smoother and can provide more accurate data for physics or rendering, reducing the appearance of large, flat polygons.

- **Mesh Expansion** : After subdivision, every vertex in the mesh is pushed slightly outwards along its normal vector. This creates a slightly larger "shell" of the scene geometry. This is a common technique to combat "Z-fighting" (flickering at the edge of occlusion) and ensure that the occlusion mesh is slightly "thicker" than the real-world surface, leading to more stable and robust occlusion.

Depth buffer to handle occlusion
## Build
> Enable Quest system property to use experimental features, you will need the command: `adb shell setprop debug.oculus.experimentalEnabled 1`.

> **Note**: This value resets with each headset reboot.
### Android
#### Dependencies
* Android Studio
  * The lastest Android Studio release is recommended.
  * If you have agreed with the licenses previously on your development system, Android Studio will automatically install, at the start up time. Otherwise (or if the installation failed), you need to install the required CMake and NDK manually, refer to the [official instructions](https://developer.android.com/studio/projects/install-ndk) for the detailed steps. The default installed locations are:
    * `$SDK-ROOT-DIR/ndk/$ndkVersion` for NDK.
    * `$SDK-ROOT-DIR/cmake/$cmake-version` for CMake.
#### Build with CMake on Android Studio
1. If this is the first time you are launching Android Studio, select Open an existing Android Studio project. If you have launched Android Studio before, click File > Open instead.
2. Open build.gradle under Samples folder
   * You could also open individual sample app from the Samples/XrSamples folders. For example, Samples/XrSamples/XrMeshOcclusion/Projects/Android/build.gradle
3. After the build has finished, directly run a sample with the device connected. Click Run in the toolbar.

### Meta Quest Link
While the samples provided in this package are designed as Quest Native / Android samples, some of them can be built and run on Windows for Meta Quest Link. For building with Meta Quest Link, you'll need:
* [cmake](https://cmake.org/download/)
* A C++ development environment compatible with cmake (such as Microsoft Visual Studio 2022)
* [Meta Quest Link application](https://www.meta.com/quest/setup/)

#### Notes for running samples with Meta Quest Link
* Ensure that Developer Runtime Features is enabled in the Meta Quest Link application.
* Make sure the headset is on, the Meta Quest Link application is running and Meta Quest Link is started; before double-click and launch the sample.
