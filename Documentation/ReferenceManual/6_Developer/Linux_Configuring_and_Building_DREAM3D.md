# Linux SDK Build Instructions #

Follow the instructions to create an SDK here.

[](https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Linux.md)

Next, download the source code repositories by following the instructions on the [SourceDownload](SourceDownload/index.html) page.

## Run CMake ##

+ If you are using _Makefiles_ or _Ninja_ as your build system you will need to decide which type of 
build you would like: **Debug** or **Release**.
+ You will need to remember where you installed the *DREAM3D_SDK*.

The following example assumes that the DREAM3D_SDK is located in **/home/[USER-NAME]/DREAM3D_SDK** and you have another directory **/home/[USER-NAME]/Workspace/** that has the DREAM3D sources:

    /home/[USER-NAME]/Workspace/CMP
    /home/[USER-NAME]/Workspace/SIMPL
    /home/[USER-NAME]/Workspace/SIMPLib
    /home/[USER-NAME]/Workspace/DREAM3D
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/Anisotropy
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/DDDAnalysisToolbox
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/DREAM3DReview
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/HEDMAnalysis
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/ImageProcessing
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/ITKImageProcessing
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/MASSIFUtilities
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/SimulationIO
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/TransformationPhase
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/UCSBUtilities
    /home/[USER-NAME]/Workspace/DREAM3D_Plugins/ZeissImport

Invoke CMake with the following command

    [user@host] $ cd /home/[USER-NAME]/Workspace
    [user@host] $ mkdir DREAM3D-Builds
    [user@host] $ cd DREAM3D-Builds
    [user@host] $ mkdir Release
    [user@host] $ cd Release
    [user@host] $ cmake -G Ninja -DDREAM3D_SDK=/PATH/TO/DREAM3D_SDK -DCMAKE_BUILD_TYPE=Release ../../DREAM3D

This will tell _CMake_ to configure a build (binary) folder (the current folder) and also generate the build files. In this example we are using the **Ninja** build system. Most Linux distributions will need to install this. You can either use your package manager (ninja-build) or download a binary from [GitHub](https://github.com/ninja-build/ninja/releases). An alternate method is to just use the default _Makefiles_ which can be done in the following way:

    [user@host] $ cd /home/[USER-NAME]/Workspace
    [user@host] $ mkdir DREAM3D-Builds
    [user@host] $ cd DREAM3D-Builds
    [user@host] $ mkdir Release
    [user@host] $ cd Release
    [user@host] $ cmake -DDREAM3D_SDK=/PATH/TO/DREAM3D_SDK -DCMAKE_BUILD_TYPE=Release ../../DREAM3D

Assuming that nothing goes wrong you are now ready to compile DREAM.3D. Using the command prompt again to issue either the _make_ or _ninja_ commands depending on what you using when configuring.

    [user@host] $ ninja

**OR**

    [user@host] $ make -j

Once the compilation is complete the DREAM.3D executable will be located in "/home/[USER-NAME]/Workspace/DREAM3D-Builds/Release/Bin". The user can then just execute the program like usual.

If you would like to build a redistributable package then from within the build directory issue the "cpack" command.

    [user@host] $ cpack.

When that ends there will be a "DREAM3D_XXXXXXXXX.tar.gz" file in the "/home/[USER-NAME]/Workspace/DREAM3D-Builds/Release" directory.

