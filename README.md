# MetaQuest Occlusion Samples

## Table of contents  
- [Introduction](#introduction)  
- [Soft Occlusion](#socclusion)
- [Mesh Occlusion](#mocclusion)
-  [How to build](#build)

## Introduction
This project is an educational resource for developers interested in Augmented Reality, focusing on the implementation of hologram occlusion with the Meta Quest.

The core technologies used are:

-   **[OpenXR API](https://registry.khronos.org/OpenXR/specs/1.0/styleguide.html):** For handling interactions with the Meta Quest hardware and its AR capabilities.
    
-   **[OpenGL ES 3.1](https://www.opengl.org/Documentation/Specs.html):** For all 3D rendering tasks, including geometry and GLSL shader programming.
    

The main objective of these samples is to provide a hands-on demonstration of how to achieve convincing occlusion of virtual content by the real world. Starting from the foundation provided by the [Meta OpenXR SDK](https://github.com/meta-quest/Meta-OpenXR-SDK), this repository explores and implements two different techniques for hologram occlusion.




## <a id="socclusion"></a> Soft Occlusion
![Alt Text](https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExcHhtODE5a2J5bmJlNXZnOW1ub2tlc21nYzF4NDE5MWg5czQ3cDQzdCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/z6EG2su1f5jOTourNL/giphy.gif)

*here is a little demo recording*

**OpenXR Extensions:** 
- XrPassthrough: This extension is what enables **Augmented Reality** on a typically **Virtual Reality** device. In simple terms, it gives developers control over the headset's built-in tracking cameras to display a live feed of the real world to the user.
- XrEnvironmentDepth: This is the "magic" that makes the interaction between the real and virtual worlds believable. While passthrough shows the real world, the environment depth functionality understands it in 3D. Specifically, this feature provides the application with a **depth map** of the user's surroundings.

**OpenGL shaders:** 
-  Vertex shader: a simple **model view projection matrix** definition.
- Fragment shader: The primary purpose of this shader is to draw a virtual object (a "hologram") in an Augmented Reality scene and make it realistically disappear behind real-world objects. To do this, it compares the depth of the virtual object against a depth map of the real world, which is provided by the Meta Quest depth sensor. The shader implements two techniques to make this occlusion look smooth and natural: **Soft Occlusion** and **Multi-Sampling**.

## <a id="mocclusion"></a> Mesh Occlusion
### TODO{
**OpenXR Extensions:**
- XrMetaSpatialEntityMesh: Provides the actual 3D triangle mesh for a spatial entity.
- XrFBSceneCapture: Provides an interface to initiate the process of scanning and "capturing" a new scene.

**OpenGL shaders:**
### TODO}

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
