# macOS Build Instructions #

Follow the instructions to create an SDK here.

[Making_an_SDK_OSX](https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_OSX.md)

Next, download the source code repositories by following the instructions on the [SourceDownload](SourceDownload/index.html) page.

## Setup ##

+ If you are using _Makefiles_ or _Ninja_ as your build system you will need to decide which type of 
build you would like: **Debug** or **Release**.
+ You will need to remember where you installed the *DREAM3D_SDK*.

The following example assumes that the DREAM3D_SDK is located in **/Users/[USER-NAME]/DREAM3D_SDK** and you have another directory **/Users/[USER-NAME]/Workspace/** that has the DREAM3D sources:

    /Users/[USER-NAME]/Workspace/CMP
    /Users/[USER-NAME]/Workspace/SIMPL
    /Users/[USER-NAME]/Workspace/SIMPLib
    /Users/[USER-NAME]/Workspace/DREAM3D
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/Anisotropy
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/DDDAnalysisToolbox
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/DREAM3DReview
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/HEDMAnalysis
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/ImageProcessing
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/ITKImageProcessing
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/MASSIFUtilities
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/SimulationIO
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/TransformationPhase
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/UCSBUtilities
    /Users/[USER-NAME]/Workspace/DREAM3D_Plugins/ZeissImport

## Run CMake ##

Invoke CMake with the following command

    [user@host] $ cd /Users/[USER-NAME]/Workspace
    [user@host] $ mkdir DREAM3D-Builds
    [user@host] $ cd DREAM3D-Builds
    [user@host] $ mkdir Release
    [user@host] $ cd Release
    [user@host] $ cmake -G Ninja -DDREAM3D_SDK=/PATH/TO/DREAM3D_SDK -DCMAKE_BUILD_TYPE=Release ../../DREAM3D

This will tell _CMake_ to configure a build (binary) folder (the current folder) and also generate the build files. In this example we are using the **Ninja** build system. Most Linux distributions will need to install this. You can either use your package manager (ninja-build) or download a binary from [GitHub](https://github.com/ninja-build/ninja/releases). An alternate method is to just use the default _Makefiles_ which can be done in the following way:

    [user@host] $ cd /Users/[USER-NAME]/Workspace
    [user@host] $ mkdir DREAM3D-Builds
    [user@host] $ cd DREAM3D-Builds
    [user@host] $ mkdir Release
    [user@host] $ cd Release
    [user@host] $ cmake -DDREAM3D_SDK=/PATH/TO/DREAM3D_SDK -DCMAKE_BUILD_TYPE=Release ../../DREAM3D

Assuming that nothing goes wrong you are now ready to compile DREAM.3D. Using the command prompt again to issue either the _make_ or _ninja_ commands depending on what you using when configuring.

    [user@host] $ ninja

or

    [user@host] $ make -j

Once the compilation is complete the DREAM.3D executable will be located in "/Users/[USER-NAME]/Workspace/DREAM3D-Builds/Release/Bin". The user can then just execute the program like usual.

## Packaging ##

If you would like to build a redistributable package then from within the build directory issue the "cpack" command.

    [user@host] $ cpack.

When that ends there will be a "DREAM3D_XXXXXXXXX.tar.gz" file in the "/Users/[USER-NAME]/Workspace/DREAM3D-Builds/Release" directory.

