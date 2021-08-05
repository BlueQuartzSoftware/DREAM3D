# Using Python to Script DREAM.3D: Building DREAM.3D from Source #

## Requirements ##

All requirements to compile DREAM.3D will apply to this technique. The major difference being that we will be using packages installed through conda-forge as the DREAM3D_SDK instead of copmiling everything from scratch.

### Clone the DREAM3D git repositories ###

You wil need to clone DREAM3D development repos into a new directory of your choice

```lang-console
(base) C:\Users\johnsmith> mkdir DREAM3D-Dev
(base) C:\Users\johnsmith> cd DREAM3D-Dev
(base) C:\Users\johnsmith\DREAM3D-Dev> git clone https://github.com/BlueQuartzSoftware/DREAM3D
(base) C:\Users\johnsmith\DREAM3D-Dev> git clone https://github.com/BlueQuartzSoftware/SIMPL
(base) C:\Users\johnsmith\DREAM3D-Dev> git clone https://github.com/BlueQuartzSoftware/SIMPLView
(base) C:\Users\johnsmith\DREAM3D-Dev> git clone https://github.com/BlueQuartzSoftware/EbsdLib
(base) C:\Users\johnsmith\DREAM3D-Dev> git clone https://github.com/BlueQuartzSoftware/H5Support
(base) C:\Users\johnsmith\DREAM3D-Dev> git clone https://github.com/BlueQuartzSoftware/CMP
(base) C:\Users\johnsmith\DREAM3D-Dev> git clone https://github.com/dream3d/DREAM3D_Data
```

We now need to clone any additional plugins that are needed. The below are the 4 standard plugins that are
used in DREAM.3D builds.
```lang-console
(base) C:\Users\johnsmith\DREAM3D-Dev> mkdir DREAM3D_Plugins
(base) C:\Users\johnsmith\DREAM3D-Dev> cd DREAM3D_Plugins
(base) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D_Plugins> git clone https://github.com/BlueQuartzSoftware/SimulationIO
(base) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D_Plugins> git clone https://github.com/BlueQuartzSoftware/ITKImageProcessing
(base) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D_Plugins> git clone https://github.com/dream3d/DREAM3DReview
(base) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D_Plugins> git clone https://github.com/dream3d/UCSBUtilities
```

If there are additional plugin repositories that you have developed, please clone them into the *DREAM3D_Plugins* directory

We need to decompress (Unzip) the following files in the DREAM3D_Data directory. Use your favorite decompression tool such as **tar** or **7Zip**.

+ DREAM3D_Data/Data/Image.tar.gz
+ DREAM3D_Data/Data/SmallN100.tar.gz
+ DREAM3D_Data/Data/T12-MAI-2010.tar.gz

### Anaconda Virtual Environment Setup ###

We are now ready to start building the DREAM3D Anaconda package.

+ Add the *conda-forge* channel and set it's priority:

```lang-console
(base) C:\Users\johnsmith> conda config --add channels conda-forge
(base) C:\Users\johnsmith> conda config --set channel_priority strict
```

+ Create the python virtual environment based on the environment.yml file in the DREAM3D repo. This will create a virtual environment using python 3.7 with a name of *dream3d*. If you already have an environment by this name then manually look at the `environment.yml` file and ensure all of the listed packages are installed.

```lang-console
(base) C:\Users\johnsmith\DREAM3D-Dev> conda env create -f DREAM3D/environment.yml
(base) C:\Users\johnsmith\DREAM3D-Dev> conda activate dream3d
(dream3d) C:\Users\johnsmith\DREAM3D-Dev>
```

Some of the example python codes use matplotlib. If you run those codes you will have to install.

### Building DREAM.3D ###

Now we need to create a build directory, configure and finally build DREAM.3D. For this we will need the following tools:

+ [CMake version 3.18 or higher](https://www.cmake.org/download)
+ [Ninja](https://github.com/ninja-build/ninja/releases)
+ Compilers for your system (MSVC on Windows, Clang on macOS, GCC/Clang on Linux)
    + Visual Studio 2017 x64 native tools on Windows 10
    + Xcode version >=12.4 (macOS 10.15 Catalina minimum version)
    + GCC version >=9.x

Please note that creating *Visual Studio* solutions using CMake is **NOT** supported. You are installing Visual Studio for the compilers only. You will use *Ninja* as the build system.

We are going to create a seperate build folder where we can place variations of the build. For this we create the folder structure `DREAM3D-Builds/Release`

```lang-console
(dream3d) C:\Users\johnsmith\DREAM3D-Dev> mkdir DREAM3D-Builds
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds> cd DREAM3D-Builds
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds> mkdir Release
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release> cd Release
```

Once we have the build directory created we can then run CMake to configure the build.

**WINDOWS COMMAND PROMPT EXAMPLE**

The following is an example of the command prompt set of commands that should configure and build DREAM.3D with python wrapping and python embedding included. 

```lang-console
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release> set LIBRARY_PREFIX=C:/Applications/Anaconda3/envs/dream3d/Library
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release> set SRC_DIR=C:\Users\mjackson\DREAM3D-Dev
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release> cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_PREFIX_PATH=%LIBRARY_PREFIX% -DQWT_INSTALL=%LIBRARY_PREFIX% -DSIMPL_WRAP_PYTHON=ON -DSIMPL_EMBED_PYTHON=ON -DDREAM3D_DISABLE_DEPENDENCY_COPY_INSTALL_RULES=ON -DDREAM3D_ANACONDA=ON -DDREAM3D_EXTRA_PLUGINS="DREAM3DReview;ITKImageProcessing;UCSBUtilities;SimulationIO" -DITKImageProcessing_ENABLE_ITK_MONTAGE=OFF -DITKImageProcessing_ENABLE_ITK_TOTAL_VARIATION=OFF -DSIMPL_USE_MULTITHREADED_ALGOS=ON -DDREAM3D_DATA_DIR=%SRC_DIR%/DREAM3D_Data ../../DREAM3D
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release> ninja
```

**The build can take up to an hour on a fast quad core system, 30 minutes on a fast 8 core system. Be patient. Go get coffee or a snack.**

Once the build is finished you will need to set/update the **PYTHONPATH** environment variable and probably the **PATH** environment variable

```lang-console
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release> set PYTHONPATH=C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release\python_packages
(dream3d) C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release> set PATH=%PATH%;C:\Users\johnsmith\DREAM3D-Dev\DREAM3D-Builds\Release\bin
```

You should now be able to run your custom python files. To get started there are a few example files in `SIMPL\Wrapping\Python\Examples`. There are also a lot of unit test files located at `DREAM3D\Test\Python\*.py` that can also be used with some minor modifications. Due to how the unit tests are run the `import` statemetns are slightly different. To use the files you will need to change the following:

```{.python}
import simpl
import simplpy
import simpl_helpers as sh
```

to 

```{.python}
# Import SIMPL wrapping
import dream3d.simpl as simpl
import dream3d.simplpy as simplpy
import dream3d.simpl_helpers as sh
```

With that change to the import statements the codes should work correctly.
