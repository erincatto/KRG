<img src="Screenshots/The Forge - Colour Black Landscape.png" width="108" height="46" />

The Forge is a cross-platform rendering framework supporting
- PC 
  * Windows 10 / 7
     * with DirectX 12 / Vulkan 1.1
     * with DirectX Ray Tracing API
     * DirectX 11 Fallback Layer for Windows 7 support
  * Linux Ubuntu 18.04 LTS with Vulkan 1.1 and RTX Ray Tracing API
- Android Pie or higher with 
  * Vulkan 1.1
  * OpenGL ES 2.0 fallback for large scale business application frameworks
- macOS / iOS / iPad OS with Metal 2.2 and M1 support
- Quest 2 using Vulkan 1.1
- XBOX One / XBOX One X / XBOX Series S/X *
- PS4 / PS4 Pro *
- PS5 *
- Switch using Vulkan 1.1 *

*(only available for accredited developers on request)

Particularly, the graphics layer of The Forge supports cross-platform
- Descriptor management. A description is on this [Wikipage](https://github.com/ConfettiFX/The-Forge/wiki/Descriptor-Management)
- Multi-threaded and asynchronous resource loading
- Shader reflection
- Multi-threaded command buffer generation

The Forge can be used to provide the rendering layer for custom next-gen game engines. It is also meant to provide building blocks to write your own game engine. It is like a "lego" set that allows you to use pieces to build a game engine quickly. The "lego" High-Level Features supported on all platforms are at the moment:
- Resource Loader as shown in 10_PixelProjectedReflections, capable to load textures, buffers and geometry data asynchronously
- [Lua Scripting System](https://www.lua.org/) - currently used in 06_Playground to load models and textures and animate the camera and in several other unit tests to cycle through the options they offer during automatic testing.
- Animation System based on [Ozz Animation System](https://github.com/guillaumeblanc/ozz-animation)
- Consistent Math Library  based on an extended version of [Vectormath](https://github.com/glampert/vectormath) with NEON intrinsics for mobile platforms
- Extended version of [EASTL](https://github.com/electronicarts/EASTL/)
- Consistent Memory Managament: 
  * on GPU following [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) and the [D3D12 Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator)
  * on CPU [Fluid Studios Memory Manager](http://www.paulnettle.com/)
- Input system with Gestures for Touch devices based on an extended version of [gainput](https://github.com/jkuhlmann/gainput)
- Fast Entity Component System based on our internally developed ECS
- Cross-platform FileSystem C API, supporting disk-based files, memory streams, and files in zip archives
- UI system based on [imGui](https://github.com/ocornut/imgui) with a dedicated unit test extended for touch input devices
- Shader Translator using a superset of HLSL as the shader language, called The Forge Shading Language. There is a Wiki page on [The Forge Shading Language](https://github.com/ConfettiFX/The-Forge/wiki/The-Forge-Shading-Language-(FSL))
- Various implementations of high-end Graphics Effects as shown in the unit tests below

Please find a link and credits for all open-source packages used at the end of this readme.

<a href="https://discord.gg/hJS54bz" target="_blank"><img src="Screenshots/Discord.png" 
alt="Twitter" width="20" height="20" border="0" /> Join the Discord channel at https://discord.gg/hJS54bz</a>

<a href="https://twitter.com/TheForge_FX?lang=en" target="_blank"><img src="Screenshots/twitter.png" 
alt="Twitter" width="20" height="20" border="0" /> Join the channel at https://twitter.com/TheForge_FX?lang=en</a>

The Forge Interactive Inc. is a [Khronos member](https://www.khronos.org/members/list)
 

# Build Status 

* Windows [![Build status](https://ci.appveyor.com/api/projects/status/leqbpaqtqj549yhh/branch/master?svg=true)](https://ci.appveyor.com/project/wolfgangfengel/the-forge/branch/master)
* macOS [![Build Status](https://app.travis-ci.com/ConfettiFX/The-Forge.svg?branch=master)](https://app.travis-ci.com/ConfettiFX/The-Forge)

# News

## Release 1.50 - October 13, 2021 - M²H uses The Forge  | Unlinked Multi GPU Support | Central Config.h | glTF Viewer improvements | Scalar High Precision Math 
* M²H uses The Forge for Stroke Therapy - [M²H](https://msquarehealthcare.com/) is a medical technology company. They developed a physics-based video game therapy solution that is backed by leading edge neuroscience, powered by Artificial Intelligence and controlled by dynamic movement – all working in concert to stimulate vast improvement of cognitive and motor functions for patients with stroke and the aged.
The Forge provides the rendering layer for their application.
Here is a YouTube video on what they do:

[![M²H on YouTube](Screenshots/M2Hscreenshot.PNG)](https://www.youtube.com/watch?v=l2Gr2Ts48e8&t=12s)


* Unlinked multiple GPU Support: for professional visualization applications, we now support unlinked multiple GPU. 
A new renderer API is added to enumerate available GPUs.
Renderer creation is extended to allow explicit GPU selection using the enumerated GPU list.
Multiple Renderers can be created this way.
The resource loader interface has been extended to support multiple Renderers.
It is initialized with the list of all Renderers created.
To select which Renderer (GPU) resources are loaded on, the NodeIndex used in linked GPU configurations is reused for the same purpose.
Resources cannot be shared on multiple Renderers however, resources must be duplicated explicitly if needed.
To retrieve generated content from one GPU to another (e.g. for presentation), a new resource loader operation is provided to schedule a transfer from a texture to a buffer. The target buffer should be mappable.
This operation requires proper synchronization with the rendering work; a semaphore can be provided to the copy operation for that purpose.
Available with Vulkan and D3D12.
For other APIs, the enumeration API will not create a RendererContext which indicates lack of unlinked multi GPU support.

* Config.h: We now have a central config.h file that can be used to configure TF.
  * Created config files:
```
Common_3/Core/Config.h
Common_3/Renderer/RendererConfig.h
Common_3/Renderer/{RenderingAPI}/{RenderingAPI}Config.h
```
        * Modified PyBuild.py
            * Proper handling of config options.
            * Every config option has --{option-name}/--no-{option-name} flag that uses define/undef directives to enable/disable macros. 
            * Macros are guarded with ifndef/ifdef.
            * Updated Android platform handling
          * Deleted Common_3/Renderer/Compiler.h. It's functionality was moved into Config.h
          * Moved all macro options to config files
          * Renamed USE_{OPTON_NAME} to ENABLE_{OPTION_NAME}
          * Changed some macros to be defined/not defined instead of having values of 0 or 1.
          * Deleted all DISABLE_{OPTION_NAME} macros
          * When detecting raytracing replaced ENABLE_RAYTRACING with RAYTRACING_AVAILABLE. This was done, because not all projects need raytracing even if it is available. RendererConfig.h defines ENABLE_RAYTRACING macro if it is available. So, it can be commented out in singular place instead of searching for it for every platform
          * Removed most of the macro definitions from build systems. Some of the remaining macros are:
            * Target platform macros: NX64, QUEST_VR
            * Arm neon macro ANDROID_ARM_NEON.
            * Windows suppression macros(like _CRT_SECURE_NO_WARNINGS)
            * Macros specific to gainputstatic
* glTF viewer improvements: 
  * sRGB fixes
  * IBL support now with prefiltered CCO/public domain cube maps
  * TAA support on more platforms and fixes
  * Vignette support


glTF Viewer running on Android Galaxy Note 9
![glTF Viewer running on Android Galaxy Note 9](Screenshots/ModelViewer/AndroidGalaxyNote9.png)

glTF Viewer running on iPhone 7
![glTF Viewer running on iPhone 7](Screenshots/ModelViewer/iPhone7.png)

glTF Viewer running on Linux with NVIDIA RTX 2060
![glTF Viewer running on Linux with NVIDIA RTX 2060](Screenshots/ModelViewer/LinuxRTX2060.png)

glTF Viewer running on Mac Mini M1
![glTF Viewer running on Mac Mini M1](Screenshots/ModelViewer/MacMiniM1.png)

glTF Viewer running on PS5
![glTF Viewer running on PS5](Screenshots/ModelViewer/PS5.png)

glTF Viewer running on Switch
![glTF Viewer running on Switch](Screenshots/ModelViewer/Switch.png)

glTF Viewer running on XBOX One Original 
![glTF Viewer running on XBOX One Original](Screenshots/ModelViewer/XBOXOneOriginal.png)


* Specialization/Function constants support on Vulkan and Metal only - these constants get baked into the micro-code during pipeline creation time so the performance is identical to using a macro without any of the downsides of macros (too many shader variations increasing the size of the build).

Good read on Specialization constants. Same things apply to function constants on Metal

https://arm-software.github.io/vulkan_best_practice_for_mobile_developers/samples/performance/specialization_constants/specialization_constants_tutorial.html

Declared at global scope using SHADER_CONSTANT macro. Used as any regular variable after declaration

Macro arguments:
```
#define SHADER_CONSTANT(INDEX, TYPE, NAME, VALUE)
```
Example usage:
```
SHADER_CONSTANT(0, uint, gRenderMode, 0);
// Vulkan - layout (constant_id = 0) const uint gRenderMode = 0;
// Metal  - constant uint gRenderMode [[function_constant(0)]];
// Others - const uint gRenderMode = 0;

void main()
{
    // Can be used like regular variables in shader code
    if (gRenderMode == 1)
    {
        // 
    }
}
```

* Resolved GitHub Issues
  * #206 - Executing Unit Tests on Mac OS 10.14 gives a Bad Access error
  * #209 - way to read texture back from GPU to CPU - this functionality is now in the resource loader
  * #210 - memory allocation challenge - not an issue
  * #212 - Question: updating partial uniform data on OpenGLES backend - not possible with OpenGL ES 2.0 run-time
  * #219 - Question : way to support Vulkan SpecializationInfo? - support is now in the code base see above


## Release 1.49 - September 09, 2021 - Quest 2 Support | Apple M1 support 
<ul>

<li>Quest 2 Support - after working now for the last 4 years on various Quest projects, we decided to add Quest 2 support to our framework. </li>


Quest 2 running 01_Transformations
![Quest 2 Running 01_Transformations](Screenshots/Quest/01_Transformations.png)

Quest 2 running 09_ShadowPlayground
![Quest 2 Running 09_ShadowPlayground](Screenshots/Quest/09_ShadowPlayground.png)

<li>At this moment the following unit tests do not work:
<ul>
  <li> 07_Tessellation: Tesselation is not supported when using Multiview. Unit test has been removed from Quest solution file. </li>
  <li>10_ScreenSpaceReflections: Lots of artifacts.</li> 
  <li>14_WaveIntrinsics: Wave intrinsics are not supported.</li>
</ul>
</ul>

<ul>
<li> Apple M1 support - we are testing now on a M1 iMac and a M1 iPad Pro. Unfortunately we have crashes in one unit test and all the more complex examples and middleware. 
</li>


iMac with M1 chip running at 3840x2160 resolution
![iMac with M1 Running 10_PerPixelProjectedReflections](Screenshots/10_Pixel-ProjectedReflections_iMacM1.png)

iPad with M1 chip running with 1024x1366 resolution
![iPad with M1 Running 10_PerPixelProjectedReflections](Screenshots/10_Pixel-ProjectedReflections_iPadM1.png)

It is astonishing how well the iPad with M1 chip perform.
Due to -what we consider driver bugs- M1 hardware crashes in 
 <ul>
 <li> Aura</li>
 <li>16_raytracing</li>
 <li>Visibility Buffer</li>
 <li>Ephemeris</li>
 </ul>
 </ul>


<ul>
<li>UI / Fonts / Lua interface refactor</li>
<ul>
<li>Moved Virtual Joystick to IInput.h / InputSystem.cpp</li>
<li>Pulled current Lua implementation out of AppUI and gave it its own interface (IScripting.h)</li>
<li>Pulled Fontstash implementation out of AppUI and gave it its own interface (IFont.h)</li>
<li>IFont and IScripting are now initialized on the OS Layer, with user customization functions available on the App Layer</li>
<li>Fonts and Lua can now be disabled via preprocessor defines and UI will still function (using default 'ProggyClean' font)</li>
</ul>
<li>Zip unit test refactor to support encryption and writes into archive</li>
For one of our customer projects we need password encryption, so we replaced our old zip library with 

[minizip ng](https://github.com/zlib-ng/minizip-ng)
<li>Extended iOS Gesture / Android gesture support</li>
For the same project we added more gesture support for mobile platforms.
<li>Partial C99 rewrite of OS/Interfaces headers and implementation files</li>
Our on-going effort to make TF easier to use is to rewrite parts in C99, so that teams can work with it more efficiently, the compile time goes down as well as the memory footprint is smaller.
<li>OpenGL ES 2 - Unit test 17 is now working as well.</li>
</ul>

<ul>
<li>GitHub fixes:</li>
<ul>
<li> Pull Request "Fix typo" #199 </li>
<li> Pull Request "Fix iOS Xcode OpenGL ES Error breakpoint crash" #202 </li>
<li> Pull Request "Reduce GL ES buffer allocation frequency" #204 </li>
<li> Pull Request "Apple silicon m1 fixes" #208 </li>
</ul>
</ul>


## Release 1.48 - May 20th, 2021 - Aura | New FSL Shader Language Translator | Run-time API Switching | Variable Rate Shading | MSAA | OpenGL ES 2 Update | PVS Studio
This is our biggest update since we started this repository more than three years ago. This update is one of those "what we have learned from the last couple of projects that are using TF" updates and a few more things.

- Aura - Dynamic Global Illumination - we developed this system in the 2010 / 2011 time frame. It is hard to believe it is 10 years ago now :-) ... it shipped in Agents of Mayhem at some point and was implemented and used in other games. We are just putting the "base" version without any game specific modifications in our commercial Middleware repository on GitHub. The games that used this system made specific modifications to the code base to align with their art asset and art style.
 In today's standards this system still fulfills the requirement of a stable rasterizer based Global Illumination system. It runs efficiently on the original XBOX One, that was the original target platform, but might require art asset modifications in a game level. 
 It works with an unlimited number of light sources with minimal memory footprint. You can also cache the reflective shadow maps for directional, point and spotlights the same way you currently cache shadow maps. At some point we did a demo running on a second generation integrated Intel GPU with 256 lights that emitted direct and indirect light and had shadow maps in 2011 at GDC? :-)
 It is best to integrate that system in a custom game engine that can cache shadow maps in an intelligent way. 

Aura - Windows DirectX 12 Geforce 980TI 1080p Driver 466.47

![Aura on Windows DX12](Screenshots/Aura/W10-D3D12-GTX980Ti-Driver_466.47.png)


Aura - Windows Vulkan Geforce 980TI 1080p Driver 466.47

![Aura on Windows Vulkan](Screenshots/Aura/W10-Vulkan-GTX980Ti-Driver_466.47.png)

Aura - Ubuntu Vulkan Geforce RTX 2080 1080p

![Aura on Ubuntu Vulkan](Screenshots/Aura/ubuntu-Vulkan-RTX2080-Driver_.png)

Aura - PS4

![Aura on Ubuntu Vulkan](Screenshots/Aura/PS4.png)

Aura - XBOX One original

![Aura on Ubuntu Vulkan](Screenshots/Aura/XboxOne.png)



- Forge Shader Language (FSL) translator - after struggeling with writing a shader translator now for 1 1/2 years, we restarted from scratch. This time we developed everything in Python, because it is cross-platform. We also picked a really "low-tech keep it simple" approach. The idea is that a small game team can actually maintain the code base and write shaders efficiently. We wanted a shader translator that translates a FSL shader to the native shader language of each of the platforms. This way whatever shader compiler is used on that platform can take over the actual job of compiling the native code.
The reason why we are doing this lies mostly in the unreliability of DXC and SPIR-V in general and also their lack of reliability if it comes to cross-platform translation. 

  There is a Wiki entry that holds a FSL language primer and general information how this works here:

[The Forge Shading Language](https://github.com/ConfettiFX/The-Forge/wiki/The-Forge-Shading-Language-(FSL))

- Run-Time API Switching - we had some sort of run-time API switching in an early version of The Forge. At the time we were not expecting this to be very useful because most game teams do not switch APIs on the fly. In the meantime we found a usage case on Android, where we have to reach a large number of devices. So we came up with a better solution that is more consistent with the overall architecture and works on at least PC and Android platforms. 
On Windows PC one can switch between DX12, Vulkan and DX11 if all are supported. On Android one can switch between Vulkan and OpenGL ES 2.0. The later allows us to target a much larger group of devices for business application frameworks. We could extend this architecture to other platforms like consoles easily.
This new API switching required us to change the rendering interfaces. So it is a breaking change to existing implementations but we think it is not much effort to upgrade and the resulting code is easier to read and maintain and overall improves the code base by being more consistent.

- Device Reset - This was implemented together with API switching. Windows forces game developers to respond to a crashing device driver by resetting the device. We implemented the functionality already in the last update here on GitHub. This update integrates it better into the OS base layer. 
We also verified that the life cycle management for Windows in each application based on the IApp interface works now for device change, device reset and for API switching so that we can cover all cases of losing and recovering the device.

  The functions for API switching and device reload and reset are:
```
void onRequestReload();
void onDeviceLost();
void onAPISwitch();
```
- Variable Rate Shading (VRS) - we implemented VRS in a new unit test 35_VariableRateShading. It is only supported by DirectX 12 on Windows and XBOX Series S / X.
In this demo, we demonstrate two main ways of setting the shading rate:

  - Per-tile Shading Rate:
Generating a shading rate lookup texture on-the-fly. Used for drawing the color palette which makes up the background. The rate decreases the further the pixels are located from the center. We can see artifacts becoming visible at aggressive rates, such as 4X4. There is also a slider in the UI to modify the center of the circle.

![Per-tile Shading Rate](Screenshots/35_VRS_1.png)

  - Per-draw Shading Rate:
The cubes are drawn by a different shading rate. They are following the Per-draw rate, which can be changed via the dropdown menu in the UI.
By using a combiner that overrides the screen rates, we ensure that cubes are drawn by an independent rate.

![Per-draw Shading Rate](Screenshots/35_VRS_2.png)
The cubes are using per-draw shading rate while the background is using per-tile shading rate.

  - Notes:
    - There is a debug view showing the shading rates and the tiles' size.
    - Per-tile method may not be available on certain GPUs even if they support the Per-draw method.
    - The tile size is enforced by the GPU and is readable, as shown in the example.
    - The shading rates available can vary based on the active GPU.



- Multi-Sample Anti-Aliasing (MSAA) - we added a dynamic way of picking MSAA to unit test 9 and the Visibility Buffer example on all platforms.

PC
![MSAA](Screenshots/MSAA.png)

PS4
![MSAA](Screenshots/MSAA_PS4.png)

PS5
![MSAA](Screenshots/MSAA_PS5.png)

* Android & OpenGL ES 2 - the OpenGL ES 2 layer for Android is now more stable and tested and closer to production code. As mentioned above on an Android phone one can switch between Vulkan and OpenGL ES 2 dyanmically if both are supported.
Now Android & OpenGL ES 2 support additionally unit test 17 - Entity Component System Test.
In general we are testing many Android phones at the moment on the low and high end of the spectrum following the two Android projects we are currently working on, which are on both ends of the spectrum.

* PVS Studio - we did another manual pass on the code base with PVS Studio -a static code analyzer- to increase code quality.




See the release notes from previous releases in the [Release section](https://github.com/ConfettiFX/The-Forge/releases).

  
# PC Windows Requirements:

1. Windows 10 

2. Drivers
* AMD / NVIDIA / Intel - latest drivers 

3. Visual Studio 2017 with Windows SDK / DirectX (you need to get it via the Visual Studio Intaller)
* Base version:
  * The minimum Windows 10 version is 1803.
  * The minimum SDK version is 1803 (10.0.17134.12).

* To use Raytracing:
  * The minimum Windows 10 version is 1809.
  * The minimum SDK version is 1809 (10.0.17763.0).

https://developer.microsoft.com/en-us/windows/downloads/sdk-archive


4. The Forge supports now as the min spec for the Vulkan SDK 1.1.82.0 and as the max spec  [1.1.114](https://vulkan.lunarg.com/sdk/home)

6. The Forge is currently tested on 
* AMD 5x, VEGA GPUs (various)
* NVIDIA GeForce 9x, 10x. 20x GPUs (various)


# macOS Requirements:

1. macOS min spec. 10.15.7

2. Xcode 12.1

3. The Forge is currently tested on the following macOS devices:
* iMac with AMD RADEON 580 (Part No. MNED2xx/A)
* iMac with M1 macOS 11.6

At this moment we do not have access to an iMac Pro or Mac Pro. We can test those either with Team Viewer access or by getting them into the office and integrating them into our build system.
We will not test any Hackintosh configuration. 


# iOS Requirements:

1. iOS 14.1

2. XCode: see macOS

To run the unit tests, The Forge requires an iOS device with an A9 or higher CPU (see [GPU Processors](https://developer.apple.com/library/content/documentation/DeviceInformation/Reference/iOSDeviceCompatibility/HardwareGPUInformation/HardwareGPUInformation.html) or see iOS_Family in this table [iOS_GPUFamily3_v3](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf)). This is required to support the hardware tessellation unit test and the ExecuteIndirect unit test (requires indirect buffer support). The Visibility Buffer doesn't run on current iOS devices because the [texture argument buffer](https://developer.apple.com/documentation/metal/fundamental_components/gpu_resources/understanding_argument_buffers) on those devices is limited to 31 (see [Metal Feature Set Table](https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf) and look for the entry "Maximum number of entries in the texture argument table, per graphics or compute function") , while on macOS it is 128, which we need for the bindless texture array. 

We are currently testing on 
* iPhone 7 (Model A1778)
* iPhone Xs Max (Model MT5D2LL/A)


# iPad OS Requirements:

1. iPadOS 14

2. XCode: see macOS

We are currently testing on:
* iPad (Model A1893)
* iPad Pro with M1 with 14.7.1


# PC Linux Requirements:

1. [Ubuntu 18.04 LTS](https://www.ubuntu.com/download/desktop) Kernel Version: 4.15.0-20-generic

2. GPU Drivers:
  * AMD GPUs: we are testing on the [Mesa RADV driver](https://launchpad.net/~paulo-miguel-dias/+archive/ubuntu/pkppa/)
  * NVIDIA GPUs: we are testing with the [NVIDIA driver](http://www.nvidia.com/object/unix.html)

3. Workspace file is provided for [codelite 12.0.6](https://codelite.org/)

4. Vulkan SDK Version 1.1.108: download the native Ubuntu Linux package for all the elements of the Vulkan SDK [LunarG Vulkan SDK Packages for Ubuntu 16.04 and 18.04](https://packages.lunarg.com/)


5. The Forge is currently tested on Ubuntu with the following GPUs:
 * AMD RADEON RX 480
 * AMD RADEON VEGA 56
 * NVIDIA GeForce 2070 RTX


# Android Requirements:

1. Android Phone with Android Pie (9.x) for Vulkan 1.1 support

2. Visual Studio 2019 (Visual Studio 2017 works too but has a bug in the build module) 

3. Android API level 23 or higher

At the moment, the Android run-time does not support the following unit tests due to -what we consider- driver bugs or lack of support:
* 09_LightShadowPlayground
* 09a_HybridRayTracing
* 11_MultiGPU
* 16_RayTracing 
* 16a_SphereTracing
* 18_VirtualTexture
* 32_Window
* 35_VariableRateShading
* Visibility Buffer 
* Aura
* Ephemeris

4. We are currently testing on 
* [Samsung S20 Ultra (Qualcomm Snapdragon 865 (Vulkan 1.1.120))](https://www.gsmarena.com/samsung_galaxy_s20_ultra_5g-10040.php) with Android 10. Please note that this version uses the Qualcomm based chipset compared to the European version that uses the Exynos chipset.
* [Samsung Galaxy Note9 (Qualcomm 845 Octa-Core (Vulkan 1.1.87))](https://www.samsung.com/us/business/support/owners/product/galaxy-note9-unlocked/) with Android 10.0. Please note this is the Qualcomm version only available in the US

## Setup Android Environment
1) Using Visual Studio Installer install "Mobile Development with C++". 
We don't need NDK that comes with this packages so that can be unchecked.

2) Download latest NDK. -- https://developer.android.com/ndk/downloads

3) Add NDK path in Visual Studio (Tools->Options->Android).


Building Shaderc library:
Open Command Prompt at directory 
```
(ndk_root)\sources\third_party\shaderc
```
And use this command,
```
..\..\..\ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk APP_STL:=c++_static APP_ABI=arm64-v8a libshaderc_combined APP_PLATFORM=android-28 -j16
```
Note: If building process gives python "import" errors, try setting this environment path variable, NDK_HOST_PYTHON = (Python_dir)\python.exe


Steps if You want to creat a new Project(--there are quite a few steps but we got you covered :) )
1) Create A new Native-Activity Application for apk or Satitc Library if you want to build a library.

2) 
- copy all the includes from pch.h to android_native_app_glue.c and add #include <malloc.h>
- Delete pch.h and main.cpp 
- unselect the Use precompiled header files option from project property.

3) Since we need to add UserMacros  
- CustomSysroot ->  $(VS_NdkRoot)\sysroot
- TRIPLE  : Refer the table below for values

For respective architecture we insert the corresponding triple value in the macro(we only support ARM and ARM64)
- ARM		arm-linux-androideabi
- ARM64		aarch64-linux-android

You can either add property sheets given by us which are included in Android_VisualStudio2017 folder, to your project's current target by selecting Add existing property sheet.
or create your own and just add those two Macros.

4) add path to include directories and uncheck inherit from parents
```
$(CustomSysroot)\usr\include
$(CustomSysroot)\usr\include\$(TRIPLE)
$(VS_Ndkroot)\sources\android\native_app_glue
```
5)add path to library 
```
$(SolutionDir)$(Platform)\$(Configuration)\ (this is where we have all our libs. set it accordingly)
$(VS_Ndkroot)\sources\third_party\shaderc\libs\c++_static\$(TargetArchAbi)
```
6)add path to executable directory and uncheck inherit from parent
```
$(VS_NdkRoot)\toolchains\llvm\prebuilt\windows-x86_64\bin
```
 
Notes:

Add -lm to you project Linker Command Line options for if you get any undefined math  operations error

If you get error related to "cannot use 'throw' with exceptions disabled"
- Enable exceptions in C++ Project settings

If you get error related to multiple instances of ioctl 
 add BIONIC_IOCTL_NO_SIGNEDNESS_OVERLOAD in preprocessor definitions
 
 If you get errors related to neon support not enabled
 -Enable Advance SIMD to Yes
 -Set floating point ABI to softfp
 

# Quest 2 Requirements:
1) Uses Visual studio 2017 with "Mobile Development with C++"  -> see Android Setup above
2) Follow steps in OS/Android/Visual Studio 2017 setup.txt for full android environment  
3) Download OVR mobile sdk from oculus website.  https://developer.oculus.com/downloads/package/oculus-mobile-sdk/  
4) Tested with ovr-mobile-sdk version 1.46  
5) Place unzipped sdk in The-Forge/Common_3/ThirdParty/OpenSource/ovr_sdk_mobile_1.46.0  
6) Download vulkan validation libs for android from https://github.com/KhronosGroup/Vulkan-ValidationLayers/releases
   a) Place unzipped sdk in The-Forge/Common_3/ThirdParty/OpenSource/Vulkan-ValidationLayer-1.2.182.0
   b) Folder should contain the following: Vulkan-ValidationLayer-1.2.182.0/arm64-v8a/libVkLayer_khronos_validation.so
7) Run examples from Examples_3/Unit_Tests/Quest_VisualStudio2017


# Install 
 * For PC Windows run PRE_BUILD.bat. It will download and unzip the art assets and install the shader builder extension for Visual Studio 2017.
 * For Linux and Mac run PRE_BUILD.command. If its the first time checking out the forge make sure the PRE_BUILD.command has the correct executable flag by running the following command
  chmod +x PRE_BUILD.command
  
    It will only download and unzip required Art Assets (No plugins/extensions install). 

# Unit Tests
There are the following unit tests in The Forge:

## 1. Transformation

This unit test just shows a simple solar system. It is our "3D game Hello World" setup for cross-platform rendering.

![Image of the Transformations Unit test](Screenshots/01_Transformations.PNG)

## 2. Compute

This unit test shows a Julia 4D fractal running in a compute shader. In the future this test will use several compute queues at once.

![Image of the Compute Shader Unit test](Screenshots/02_Compute.PNG)

## 3. Multi-Threaded Rendering

This unit test shows how to generate a large number of command buffers on all platforms supported by The Forge. This unit test is based on [a demo by Intel called Stardust](https://software.intel.com/en-us/articles/using-vulkan-graphics-api-to-render-a-cloud-of-animated-particles-in-stardust-application).

![Image of the Multi-Threaded command buffer generation example](Screenshots/03_MultiThreading.PNG)

## 4. ExecuteIndirect

This unit test shows the difference in speed between Instanced Rendering, using ExecuteIndirect with CPU update of the indirect argument buffers and using ExecuteIndirect with GPU update of the indirect argument buffers.
This unit test is based on [the Asteroids example by Intel](https://software.intel.com/en-us/articles/asteroids-and-directx-12-performance-and-power-savings).

![Image of the ExecuteIndirect Unit test](Screenshots/04_ExecuteIndirect.PNG)
Using ExecuteIndirect with GPU updates for the indirect argument buffers

![Image of the ExecuteIndirect Unit test](Screenshots/04_ExecuteIndirect_2.PNG)
Using ExecuteIndirect with CPU updates for the indirect argument buffers

![Image of the ExecuteIndirect Unit test](Screenshots/04_ExecuteIndirect_3.PNG)
Using Instanced Rendering

## 5. Font Rendering

This unit test shows the current state of our font rendering library that is based on several open-source libraries.

![Image of the Font Rendering Unit test](Screenshots/05_FontRendering.PNG)

## 6. Material Playground

This unit test shows a range of game related materials:

Hair:
Many years ago in 2012 / 2013, we helped AMD and Crystal Dynamics with the development of TressFX for Tomb Raider. We also wrote an article about the implementation in GPU Pro 5 and gave a few joint presentations on conferences like FMX. At the end of last year we revisited TressFX. We took the current code in the GitHub repository, changed it a bit and ported it to The Forge. It now runs on PC with DirectX 12 / Vulkan, macOS and iOS with Metal 2 and on the XBOX One. We also created a few new hair assets so that we can showcase it. Here is a screenshot of our programmer art:

![Hair on PC](Screenshots/MaterialPlayground/06_MaterialPlayground_Hair_closup.gif)

Metal:

![Material Playground Metal on PC](Screenshots/MaterialPlayground/06_MaterialPlayground_Metal.png)

Wood:

![Material Playground Wood on PC](Screenshots/MaterialPlayground/06_MaterialPlayground_Wood.png)

## 7. Hardware Tessellation

This unit test showcases the rendering of grass with the help of hardware tessellation.

![Image of the Hardware Tessellation Unit test](Screenshots/07_Hardware_Tessellation.PNG)

## 8. glTF Model Viewer
A cross-platform glTF model viewer that optimizes the vertex and index layout for the underlying platform and picks the right texture format for the underlying platform. 
This modelviewer can utilize Binomials [Basis Universal Texture Support](https://github.com/binomialLLC/basis_universal) as an option to load textures. Support was added to the Image class as a "new image format". So you can pick basis like you can pick DDS or KTX. For iOS / Android we go directly to ASTC because Basis doesn't support ASTC at the moment.

glTF Viewer running on Android Galaxy Note 9
![glTF Viewer running on Android Galaxy Note 9](Screenshots/ModelViewer/AndroidGalaxyNote9.png)

glTF Viewer running on iPhone 7
![glTF Viewer running on iPhone 7](Screenshots/ModelViewer/iPhone7.png)

glTF Viewer running on Linux with NVIDIA RTX 2060
![glTF Viewer running on Linux with NVIDIA RTX 2060](Screenshots/ModelViewer/LinuxRTX2060.png)

glTF Viewer running on Mac Mini M1
![glTF Viewer running on Mac Mini M1](Screenshots/ModelViewer/MacMiniM1.png)

glTF Viewer running on PS5
![glTF Viewer running on PS5](Screenshots/ModelViewer/PS5.png)

glTF Viewer running on Switch
![glTF Viewer running on Switch](Screenshots/ModelViewer/Switch.png)

glTF Viewer running on XBOX One Original 
![glTF Viewer running on XBOX One Original](Screenshots/ModelViewer/XBOXOneOriginal.png)


## 9. Light and Shadow Playground
This unit test shows various shadow and lighting techniques that can be chosen from a drop down menu. There will be more in the future.

 * Exponential Shadow Map - this is based on [Marco Salvi's](https://pixelstoomany.wordpress.com/category/shadows/exponential-shadow-maps/) @marcosalvi papers. This technique filters out the edge of the shadow map by approximating the shadow test using exponential function that involves three subjects: the depth value rendered by the light source, the actual depth value that is being tested against, and the constant value defined by the user to control the softness of the shadow
  * Adaptive Shadow Map with Parallax Correction Cache - this is based on the article "Parallax-Corrected Cached Shadow Maps" by Pavlo Turchyn in [GPU Zen 2](https://gpuzen.blogspot.com/2019/05/gpu-zen-2-parallax-corrected-cached.htm). It adaptively chooses which light source view to be used when rendering a shadow map based on a hiearchical grid structure. The grid structure is constantly updated depending on the user's point of view and it uses caching system that only renders uncovered part of the scene. The algorithm greatly reduce shadow aliasing that is normally found in traditional shadow map due to insufficient resolution. Pavlo Turchyn's paper from GPU Pro 2 added an additional improvement by implementing multi resolution filtering, a technique that approximates larger size PCF kernel using multiple mipmaps to achieve cheap soft shadow. He also describes how he integrated a Parallax Correction Cache to Adaptive Shadow Map, an algorithm that approximates moving sun's shadow on static scene without rendering tiles of shadow map every frame. The algorithm is generally used in an open world game to approximate the simulation of day & night’s shadow cycle more realistically without too much CPU/GPU cost.
  * Signed Distance Field Soft Shadow - this is based on [Daniel Wright's Siggraph 2015](http://advances.realtimerendering.com/s2015/DynamicOcclusionWithSignedDistanceFields.pdf) @EpicShaders presentation. To achieve real time SDF shadow, we store the distance to the nearest surface for every unique Meshes to a 3D volume texture atlas. The Mesh SDF is generated offline using triangle ray tracing, and half precision float 3D volume texture atlas is accurate enough to represent 3D meshes with SDF. The current implementation only supports rigid meshes and uniform transformations (non-uniform scale is not supported). An approximate cone intersection can be achieved  by measuring the closest distance of a passed ray to an occluder which gives us a cheap soft shadow when using SDF.

To achieve  high-performance, the playground runs on our signature rendering architecture called Triangle Visibility Buffer. The step that generates the SDF data also uses this architecture.

Click on the following screenshot to see a movie:

[![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_Visualize.png)](https://vimeo.com/352985038)

The following PC screenshots are taken on Windows 10 with a AMD RX550 GPU (driver 19.7.1) with a resolution of 1920x1080. 

Exponential Shadow Maps:

![Light and Shadow Playground - Exponential Shadow Map](Screenshots/LightNShadowPlayground/ExponentialShadowMap.png)

Adaptive Shadow Map with Parallax Correction Cache

![Adaptive Shadow Map with Parallax Correction Cache](Screenshots/LightNShadowPlayground/ASM_Two.png)

Signed Distance Field Soft Shadow:

![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_1.png)

Signed Distance Field Soft Shadows - Debug Visualization

![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_Visualize.png)

The following shots show Signed Distance Field Soft Shadows running on iMac with a AMD RADEON Pro 580

![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_macOS_1.png)

![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_macOS_2.png)

The following shots show Signed Distance Field Soft Shadows running on XBOX One:

![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_XBOX_1.png)

![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_XBOX_2.png)

![Signed Distance Field Soft Shadow Map](Screenshots/LightNShadowPlayground/SDF_XBOX_3.png)

Readme for Signed Distance Field Soft Shadow Maps:

To generate the SDF Mesh data you should select “Signed Distance Field” as the selected shadow type in the Light and Shadow Playground. There is a button called “Generate Missing SDF” and once its clicked, it shows a progress bar that represents the remaining SDF mesh objects utilized for SDF data generation. This process is multithreaded, so the user can still move around the scene while waiting for the SDF process to be finished. This is a long process and it could consume up to 8+ hours depending on your CPU specs. To check how many SDF objects there are presently in the scene, you can mark the checkbox "Visualize SDF Geometry On The Scene".

## 9a. Hybrid Ray-Traced Shadows
This unit test was build by Kostas Anagnostou @KostasAAA to show how to ray trace shadows without using a ray tracing API like DXR / RTX. It should run on all GPUs (not just NVIDIA RTX GPUs) and the expectation is that it should run comparable with a DXR / RTX based version even on a NVIDIA RTX GPU. That means the users of your game do not have to buy a NVIDIA RTX GPU to enjoy HRT shadows :-)
![Hybrid Ray Traced Shadows](Screenshots/09a_HRT_Shadows.png)


## 10. Screen-Space Reflections
This test offers two choices: you can pick either Pixel Projected Reflections or AMD's FX Stochastic Screen Space Reflection. We just made AMD's FX code cross-platform. It runs now on Windows, Linux, macOS, Switch, PS and XBOX.

Here are the screenshots of AMD's FX Stochastic Screen Space Reflections:

Windows final scene:
![AMD FX Stochastic Screen Space Reflections](Screenshots/SSSR/SSSR_Scene_with_reflections.png)

Without denoising:
![AMD FX Stochastic Screen Space Reflections before denoise](Screenshots/SSSR/SSSR_Reflections_only_defore_denoise.png)

With denoising:
![AMD FX Stochastic Screen Space Reflections before denoise](Screenshots/SSSR/SSSR_Reflections_with_denoise.png)

PS4:
![AMD FX Stochastic Screen Space Reflections on PS4](Screenshots/SSSR/SSSR_on_PS4.png)

macOS:
![AMD FX Stochastic Screen Space Reflections on macOS](Screenshots/SSSR/SSSR_on_macOS.png)

In case you pick Pixel-Projected Reflections, the application features an implementation of the papers [Optimized pixel-projected reflections for planar reflectors](http://advances.realtimerendering.com/s2017/PixelProjectedReflectionsAC_v_1.92.pdf) and [IMPLEMENTATION OF OPTIMIZED PIXEL-PROJECTED REFLECTIONS FOR PLANAR REFLECTORS](https://github.com/byumjin/Jin-Engine-2.1/blob/master/%5BByumjin%20Kim%5D%20Master%20Thesis_Final.pdf)

![Image of the Pixel-Projected Reflections Unit test](Screenshots/10_Pixel-ProjectedReflections.png)

## 11. Multi-GPU (Driver support only on PC Windows)
This unit test shows a typical VR Multi-GPU configuration. One eye is rendered by one GPU and the other eye by the other one.

![Image of the Multi-GPU Unit test](Screenshots/11_MultiGPU.png)

## 12. File System Test
This unit test showcases a cross-platform FileSystem C API, supporting disk-based files, memory streams, and files in zip archives. The API can be viewed in [IFileSystem.h](/Common_3/Interfaces/IFileSystem.h), and all of the example code has been updated to use the new API.
   * The API is based around `Path`s, where each `Path` represents an absolute, canonical path string on a particular file system. You can query information about the files at `Path`s, open files as `FileStream`s, and copy files between different `Path`s.
   * The concept of `FileSystemRoot`s has been replaced by `ResourceDirectory`s. `ResourceDirectory`s are predefined directories where resources are expected to exist, and there are convenience functions to open files in resource directories. If your resources don’t exist within the default directory for a particular resource type, you can call `fsSetPathForResourceDirectory` to relocate the resource directory; see the unit tests for sample code on how to do this.
   
![File System Unit Test](Screenshots/12_FileSystem.png)

## 13. imGUI integration unit test
This unit test shows how the integration of imGui is done with a wide range of functionality.

![Image of the imGui Integration in The Forge](Screenshots/13_imGui.gif)


## 14. Order-Independent Transparency unit test
This unit test compares various Order-Indpendent Transparency Methods. In the moment it shows:
- Alpha blended transparency
- Weighted blended Order Independent Transparency [Morgan McGuire Blog Entry 2014](http://casual-effects.blogspot.com/2014/03/weighted-blended-order-independent.html) and [Morgan McGuire Blog Entry 2015](http://casual-effects.blogspot.com/2015/03/implemented-weighted-blended-order.html)
- Weighted blended Order Independent Transparency by Volition [GDC 2018 Talk](https://www.gdcvault.com/play/1025400/Rendering-Technology-in-Agents-of)
- Adaptive Order Independent Transparency with Raster Order Views [paper by Intel, supports DirectX 11, 12 only](https://software.intel.com/en-us/articles/oit-approximation-with-pixel-synchronization-update-2014), and a [Primer](https://software.intel.com/en-us/gamedev/articles/rasterizer-order-views-101-a-primer)
- Phenomenological Transparency - Diffusion, Refraction, Shadows by [Morgan McGuire](https://casual-effects.com/research/McGuire2017Transparency/McGuire2017Transparency.pdf)
![Image of the Order-Indpendent Transparency unit test in The Forge](Screenshots/14_OIT.png)


## 15. Wave Intrinsics unit test
This unit test shows how to use the new wave intrinsics. Supporting Windows with DirectX 12 / Vulkan, Linux with Vulkan and macOS / iOS.

![Image of the Wave Intrinsics unit test in The Forge](Screenshots/15_WaveIntrinsics.png)

## 16. Path Tracer - Ray Tracing Unit Test
The new 16_Raytracing unit test shows a simple cross-platform path tracer. On iOS this path tracer requires A11 or higher. It is meant to be used in tools in the future and doesn't run in real-time.
To support the new path tracer, the Metal raytracing backend has been overhauled to use a sort-and-dispatch based approach, enabling efficient support for multiple hit groups and miss shaders. The most significant limitation for raytracing on Metal is that only tail recursion is supported, which can be worked around using larger per-ray payloads and splitting up shaders into sub-shaders after each TraceRay call; see the Metal shaders used for 16_Raytracing for an example on how this can be done.

macOS 1920x1080 AMD Pro Vega 64

![Path Tracer running on macOS](Screenshots/16_Path_Tracer_macOS.png)

iOS iPhone X 812x375

![Path Tracer running on macOS](Screenshots/16_Path_Tracer_iOS.jpeg)

Windows 10 1080p NVIDIA RTX 2080 with DXR Driver version 441.12

![Path Tracer running on Windows DXR](Screenshots/16_Path_Tracer_DXR.png)

Windows 10 1080p NVIDIA RTX 2080 with RTX Driver version 441.12

![Path Tracer running on Windows RTX](Screenshots/16_Path_Tracer_RTX.png)

Linux 1080p NVIDIA RTX 2060 with RTX Driver version 435

![Path Tracer running on Linux RTX](Screenshots/16_Path_Tracer_Linux_RTX.png)

## 16a. Sphere Tracing
This unit test was originally posted on ShaderToy by [Inigo Quilez](https://www.shadertoy.com/view/Xds3zN) and [Sopyer](https://sopyer.github.io/b/post/vulkan-shader-sample/). It shows how a scene is ray marched with shadows, reflections and AO

![Image of the Sphere Tracing  unit test in The Forge](Screenshots/16_RayMarching_Linux.png)

## 17. Entity Component System Test
This unit test shows how to use the high-performance entity component system in The Forge. This unit test is based on a ECS system that we developed internally for tools.

![Image of the Entity Component System unit test in The Forge](Screenshots/17_EntityComponentSystem.png)

## 18. Sparse Virtual Textures
The Forge has now support for Sparse Virtual Textures on Windows and Linux with DirectX 12 / Vulkan. Sparse texture (also known as "virtual texture", “tiled texture”, or “mega-texture”) is a technique to load huge size (such as 16k x 16k or more) textures in GPU memory.
It breaks an original texture down into small square or rectangular tiles to load only visible part of them.

The unit test 18_Virtual_Texture is using 7 sparse textures:
* Mercury: 8192 x 4096
* Venus: 8192 x 4096
* Earth: 8192 x 4096
* Moon: 16384 x 8192
* Mars: 8192 x 4096
* Jupiter: 4096 x 2048
* Saturn: 4096 x 4096

There is a unit test that shows a solar system where you can approach planets with Sparse Virtual Textures attached and the resolution of the texture will increase when you approach.

Linux 1080p NVIDIA RTX 2060 with RTX Driver version 435

![Sparse Virtual Texture on Linux Vulkan](Screenshots/Virtual_Texture_Linux.png) 

Windows 10 1080p NVIDIA 1080 DirectX 12

![Sparse Virtual Texture on Windows 10 DirectX 12](Screenshots/Virtual_Texture.png) 

Windows 10 1080p NVIDIA 1080 Vulkan

![Sparse Virtual Texture on Windows Vulkan](Screenshots/Virtual_Texture_VULKAN_1920_1080_GTX1080.png) 

![Sparse Virtual Texture on Windows Vulkan](Screenshots/Virtual_Texture_VULKAN_1920_1080_GTX1080_CloseUP.png) 


## 21. Ozz Playback Animation
This unit test shows how to playback a clip on a rig.

![Image of Playback Animation in The Forge](Screenshots/01_Playback.gif)

## 22. Ozz Playback Blending
This unit test shows how to blend multiple clips and play them back on a rig.

![Image of Playback Blending in The Forge](Screenshots/02_Blending.gif)

## 23. Ozz Joint Attachment
This unit test shows how to attach an object to a rig which is being posed by an animation.

![Image of Ozz Joint Attachment in The Forge](Screenshots/03_JointAttachment.gif)

## 24. Ozz Partial Blending
This unit test shows how to blend clips having each only effect a certain portion of joints.

![Image of Ozz Partial Blending in The Forge](Screenshots/04_PartialBlending.gif)

## 25. Ozz Additive Blending
This unit test shows how to introduce an additive clip onto another clip and play the result on a rig.

![Image of Ozz Additive Blending in The Forge](Screenshots/05_Additive.gif)

## 26. Ozz Baked Physics
This unit test shows how to use a scene of a physics interaction that has been baked into an animation and play it back on a rig.

![Image of Ozz Baked Physics in The Forge](Screenshots/07_BakedPhysics.gif)

## 27. Ozz Multi Threading
This unit test shows how to animate multiple rigs simultaneously while using multi-threading for the animation updates.

![Image of Ozz Multi Threading in The Forge](Screenshots/09_MultiThread.gif)

## 28. Ozz Skinning
This unit test shows how to use skinning with Ozz

![Image of the Ozz Skinning unit test](Screenshots/Skinning_PC.gif)

## 29. Ozz Inverse Kinematic
This unit test shows how to use a Aim and a Two bone IK solvers

Aim IK
![Ozz Aim IK](Screenshots/Ozz_Aim_IK.gif)

Two Bone IK
![Ozz Two Bone IK](Screenshots/Ozz_two_bone_ik.gif)

## 32. Windows Management
This test demonstrates windows management on Windows, Linux and macOS. 
  * The window layout, position, and size are now driven by the client dimensions, meaning that
the values that the client demands are the exact values the client area will be represented with, regardless of the window style. This allows for much greater flexibility
and consistency, especially when working with a fullscreen window. 
  * Multi-monitor support has also been improved significantly, offering smooth consistent transitions between client displays and guaranteeing correct window behavior and data retention. Media layer functionality has been expanded, allowing the client to control mouse positioning, mouse visibility, and mouse visual representation. 
  * It is now possible to create independent mouse cursors to further customize the application.

Here are the screenshots:

Windows:
![Windows Management for Windows](Screenshots/32_Window_Win.png)

macOS:
![Windows Management for macOS](Screenshots/32_Window_macOS.png)

Linux:
![Windows Management for Linux](Screenshots/32_Window_Linux.jpg)

## 33. YUV Support
YUV support: we have now YUV support for all our Vulkan API platforms PC, Linux, Android and Switch. There is a new functional test for YUV. It runs on all these platforms:

![YUV unit test](Screenshots/34_YUV.png)


## 35. Variable Shading Rate

 - Per tile Shading Rate
Generating a shading rate lookup texture on-the-fly. Used for drawing the color palette which makes up the background. The rate decreases the further the pixels are located from the center. We can see artifacts becoming visible at aggressive rates, such as 4X4. There is also a slider in the UI to modify the center of the circle.

![Per-tile Shading Rate](Screenshots/35_VRS_1.png)

  - Per-draw Shading Rate:
The cubes are drawn by a different shading rate. They are following the Per-draw rate, which can be changed via the dropdown menu in the UI.
By using a combiner that overrides the screen rates, we ensure that cubes are drawn by an independent rate.

![Per-draw Shading Rate](Screenshots/35_VRS_2.png)
The cubes are using per-draw shading rate while the background is using per-tile shading rate.

  - Notes:
    - There is a debug view showing the shading rates and the tiles' size.
    - Per-tile method may not be available on certain GPUs even if they support the Per-draw method.
    - The tile size is enforced by the GPU and is readable, as shown in the example.
    - The shading rates available can vary based on the active GPU.


# Examples
There is an example implementation of the Triangle Visibility Buffer as covered in various conference talks. [Here](https://diaryofagraphicsprogrammer.blogspot.com/2018/03/triangle-visibility-buffer.html) is a blog entry that details the implementation in The Forge.

![Image of the Visibility Buffer](Screenshots/Visibility_Buffer.png)


# Tools
Below are screenshots and descriptions of some of the tools we integrated.

## MTuner
MTuner
MTuner was integrated into the Windows 10 runtime of The Forge following a request for more in-depth memory profiling capabilities by one of the developers we support. It has been adapted to work closely with our framework and its existing memory tracking capabilities to provide a complete picture of a given application’s memory usage. 

To use The Forge’s MTuner functionality, simply drag and drop the .MTuner file generated alongside your application’s executable into the MTuner host app, and you can immediately begin analyzing your program’s memory usage. The intuitive interface and exhaustive supply of allocation info contained in a single capture file makes it easy to identify usage patterns and hotspots, as well as tracking memory leaks down to the file and line number. The full documentation of MTuner can be found [here](link: https://milostosic.github.io/MTuner/).

Currently, this feature is only available on Windows 10, but support for additional platforms provided by The Forge is forthcoming.
Here is a screenshot of an example capture done on our first Unit Test, 01_Transformations:
![MTuner](Screenshots/MTuner.png) 

## Ray Tracing Benchmark
Based on request we are providing a Ray Tracing Benchmark in 16_RayTracing. It allows you to compare the performance of three platforms: 
  * Windows with DirectX 12 DXR
  * Windows with Vulkan RTX
  * Linux with Vulkan RTX

  We will extend this benchmark to the non-public platforms we support to compare the PC performance with console performance. 
  The benchmark comes with batch files for all three platforms. Each run generates a HTML output file from the profiler that is integrated in TF. The default number of iterations is 64 but you can adjust that.  There is a Readme file in the 16_RayTracing folder that describes the options.

Windows DirectX 12 DXR, GeForce RTX 2070 Super, 3840x1600, NVIDIA Driver 441.99

![Windows DXR output of Ray Tracing Benchmark](Screenshots/16_Path_Tracer_Profile_DX.PNG) 

Windows Vulkan RTX, GeForce RTX 2070 Super, 3840x1600, NVIDIA Driver 441.99

![Windows RTX output of Ray Tracing Benchmark](Screenshots/16_Path_Tracer_Profile.PNG) 


## Microprofiler
We integrated the [Micro Profiler](https://github.com/zeux/microprofile) into our code base by replacing the proprietary UI with imGUI and simplified the usage. Now it is much more tightly and consistently integrated in our code base.

Here are screenshots of the Microprofiler running the Visibility Buffer on PC:

![Microprofiler](Screenshots/MicroProfiler/VB_Detailed.png)

![Microprofiler](Screenshots/MicroProfiler/VB_Plot.PNG)

![Microprofiler](Screenshots/MicroProfiler/VB_Timer.PNG)

![Microprofiler](Screenshots/MicroProfiler/VB_Timer_2.PNG)

Here are screenshots of the Microprofiler running a unit test on iOS:

![Microprofiler](Screenshots/MicroProfiler/IMG_0004_iOS.PNG)

![Microprofiler](Screenshots/MicroProfiler/IMG_0005_iOS.PNG)

![Microprofiler](Screenshots/MicroProfiler/IMG_0006_iOS.PNG)

Check out the [Wikipage](https://github.com/ConfettiFX/The-Forge/wiki/Microprofiler---How-to-Use) for an explanation on how to use it.

## Shader Translator
We provide a shader translator, that translates one shader language -a superset of HLSL called Forge Shader Language (FLS) - to the target shader language of all our target platforms. That includes the console and mobile platforms as well.
It is written in Python. We expect this shader translator to be an easier to maintain solution for smaller game teams because it allows to add additional data to the shader source file with less effort. Such data could be for example a bucket classification or different shaders for different capability levels of the underlying platform, descriptor memory requirements or resource memory requirements in general, material info or just information to easier pre-compile pipelines.
The actual shader compilation will be done by the native compiler of the target platform.

 [How to use the Shader Translator](https://github.com/ConfettiFX/The-Forge/wiki/How-to-Use-The-Shader-Translator)



# Releases / Maintenance
The Forge Interactive Inc. will prepare releases when all the platforms are stable and running and push them to this GitHub repository. Up until a release, development will happen on internal servers. This is to sync up the console, mobile, macOS and PC versions of the source code.

# Products
We would appreciate it if you could send us a link in case your product uses The Forge. Here are the ones we received so far or we contributed to:

## Supergiant Games Hades
[Supergiant's Hades](https://www.supergiantgames.com/games/hades/) we are working with Supergiant since 2014. One of the on-going challenges was that their run-time was written in C#. At the beginning of last year, we suggested to help them in building a new cross-platform game engine in C/C++ from scratch with The Forge. The project started in April 2019 and the first version of this new engine launched in May this year. Hades was then released for Microsoft Windows, macOS, and Nintendo Switch on September 17, 2020. The game can run on all platforms supported by The Forge.

Here is a screenshot of Hades running on Switch:

![Supergiant Hades](Screenshots/Supergiant_Hades.jpg)

Here is an article by [Forbes](https://www.forbes.com/sites/davidthier/2020/09/27/you-need-to-play-the-game-at-the-top-of-the-nintendo-switch-charts/#6e9128ba2f80) about Hades being at the top of the Nintendo Switch Charts.
Hades is also a technology showcase for Intel's integrated GPUs on macOS and Windows. The target group of the game seems to often own those GPUs.

## Bethesda's Creation Engine
Bethesda based their rendering layer for their next-gen engine on The Forge. We helped integrate and optimize it. 

![Bethesda's Creation Engine](Screenshots/Starfield-The-Elder-Scrolls-6-Bethesda.jpg)

Here is more info about this game engine:

[Todd Howard Teases Bethesda's New Game Engine Behind The Elder Scrolls 6 And Starfield](https://www.thegamer.com/starfield-the-elder-scrolls-6-new-game-engine/)

[Bethesda's overhauling its engine for Starfield and The Elder Scrolls 6](https://www.gamesradar.com/bethesda-engine-starfield-elder-scrolls-6/)

## M²H - Stroke Therapy
M²H uses The Forge - [M²H](https://msquarehealthcare.com/) is a medical technology company. They have developed a physics-based video game therapy solution that is backed by leading edge neuroscience, powered by Artificial Intelligence and controlled by dynamic movement – all working in concert to stimulate vast improvement of cognitive and motor functions for patients with stroke and the aged.
The Forge provides the rendering layer for their application.
Here is a YouTube video on what they do:

[![M²H on YouTube](Screenshots/M2Hscreenshot.PNG)](https://www.youtube.com/watch?v=l2Gr2Ts48e8&t=12s)

## StarVR One SDK
The Forge is used to build the StarVR One SDK:

<a href="https://www.starvr.com" target="_blank"><img src="Screenshots/StarVR.PNG" 
alt="StarVR" width="300" height="159" border="0" /></a>


## Torque 3D
The Forge will be used as the rendering framework in Torque 3D:

<a href="http://www.garagegames.com/products/torque-3d" target="_blank"><img src="Screenshots/Torque-Logo_H.png" 
alt="Torque 3D" width="417" height="106" border="0" /></a>

## Star Wars Galaxies Level Editor
SWB is an editor for the 2003 game 'Star Wars Galaxies' that can edit terrains, scenes, particles and import/export models via FBX. The editor uses an engine called 'atlas' that will be made open source in the future. It focuses on making efficient use of the new graphics APIs (with help from The-Forge!), ease-of-use and terrain rendering.

![SWB Level Editor](Screenshots/SWB.png)

# Writing Guidelines
For contributions to The Forge we apply the following writing guidelines:
 * We limit all code to C++ 11 by setting the Clang and other compiler flags
 * We follow the [Orthodox C++ guidelines] (https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b) minus C++ 14 support (see above)
 * Please note that we are going to move towards C99 usage more and more because this language makes it easier to develop high-performance applications in a team. With the increased call numbers of modern APIs and the always performance-detoriating C++ features, C++ is becoming more and more a productivity and run-time performance challenge. C is also a better starting point to port to other languages like RUST. In case any of those languages become common in development.

# User Group Meetings 
There will be a user group meeting during GDC. In case you want to organize a user group meeting in your country / town at any other point in time, we would like to support this. We could send an engineer for a talk.

# Support for Education 
In case your School / College / University uses The Forge for education, we would like to support this as well. We could send an engineer or help create material. So far the following schools use The Forge for teaching:

[Breda University of Applied Sciences](https://www.buas.nl) 
```
        Contact:
        Jeremiah van Oosten 
        Monseigneur Hopmansstraat 1
        4817 JT Breda
 ```
[Ontario Tech University](https://uoit.ca/) 
```
        Contact:
        Andrew Hogue
        Ontario Tech University
        SIRC 4th floor
        2000 Simcoe St N
        Oshawa, ON, L1H 7K4
 ```


# Open-Source Libraries
The Forge utilizes the following Open-Source libraries:
* [Fontstash](https://github.com/memononen/fontstash)
* [Vectormath](https://github.com/glampert/vectormath)
* [Nothings](https://github.com/nothings/stb) single file libs 
  * [stb.h](https://github.com/nothings/stb/blob/master/stb.h)
  * [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h)
  * [stb_image_resize.h](https://github.com/nothings/stb/blob/master/stb_image_resize.h)
  * [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)
* [shaderc](https://github.com/google/shaderc)
* [SPIRV_Cross](https://github.com/KhronosGroup/SPIRV-Cross)
* [TinyEXR](https://github.com/syoyo/tinyexr)
* [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
* [D3D12 Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator)
* [GeometryFX](https://gpuopen.com/gaming-product/geometryfx/)
* [WinPixEventRuntime](https://blogs.msdn.microsoft.com/pix/winpixeventruntime/)
* [Fluid Studios Memory Manager](http://www.paulnettle.com/)
* [volk Metaloader for Vulkan](https://github.com/zeux/volk)
* [gainput](https://github.com/jkuhlmann/gainput)
* [imGui](https://github.com/ocornut/imgui)
* [DirectX Shader Compiler](https://github.com/Microsoft/DirectXShaderCompiler)
* [Ozz Animation System](https://github.com/guillaumeblanc/ozz-animation)
* [Lua Scripting System](https://www.lua.org/)
* [TressFX](https://github.com/GPUOpen-Effects/TressFX)
* [Micro Profiler](https://github.com/zeux/microprofile)
* [MTuner](https://github.com/milostosic/MTuner) 
* [EASTL](https://github.com/electronicarts/EASTL/)
* [meshoptimizer](https://github.com/zeux/meshoptimizer)
* [Basis Universal Texture Support](https://github.com/binomialLLC/basis_universal)
* [TinyImageFormat](https://github.com/DeanoC/tiny_imageformat)
* [minizip ng](https://github.com/zlib-ng/minizip-ng)
