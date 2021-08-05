# DREAM3D and Python (Anaconda Python) #

In order to use DREAM3D from within a Python environment you will need to pick one of two options:

+ 1: Install a prebuilt 'dream3d-conda' package
+ 2: Build DREAM.3D from source using the ananconda/condaforge environment


## Install a Prebuilt 'dream3d-conda' Package ##

See the instructions in the file python\_filter\_development.md

### Writing a DREAM3D Filter in Python ###

Please see the file _conda\_filter\_development.md_ file for instructions.

## Building a DREAM3D Anaconda Package ##

### Requirements ###

+ Install [Visual Studio](https://visualstudio.microsoft.com/downloads/) (Windows)
+ Install [Anaconda](https://docs.anaconda.com/anaconda/install/)
+ Setup [conda-forge](https://conda-forge.org/) channel (requires conda >= 4.6)

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

We need to decompress (Unzip) the following in DREAM3D_Data directory. Use your favorite decompression tool such as **tar** or **7Zip**.

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

+ Next we need to create an anaconda virtual environment where we will build and eventually install the **dream3d** package. In this example we are calling our virtual environment **d3d_embed** and we are going to use python version 3.7. Create a new conda virtual environment:

```lang-console
(base) C:\Users\johnsmith> conda create -n d3d_embed python=3.7
(base) C:\Users\johnsmith> conda activate d3d_embed
(d3d_embed) C:\Users\johnsmith> 
```

+ Now that the virtual environment is setup we need to install the **conda-build** tooling:

```lang-console
(d3d_embed) C:\Users\johnsmith> conda install conda-build
```

+ Use conda build DREAM3D within the **d3d_embded** virtual environment:

The build takes around 15 min to complete on a 12 core machine with SSD storage.

The initial "solving environment" step may take a while in particular, and it does not have a progress indicator.

```lang-console
(d3d_embed) C:\Users\johnsmith> conda build /path/to/DREAM3D/Conda
```

Install locally built *DREAM3D* package into the current virtual environment

+ Make sure the Python version of the environment matches the version used to build DREAM3D.

```lang-console
(d3d_embed) C:\Users\johnsmith> conda install -c local dream3d
```

## Running DREAM3D from Anaconda ##

DREAM3D, and related applications, must be run from within the conda environment.

i.e.:

```lang-console
(base) C:\Users\johnsmith> conda activate d3d_embed
(d3d_embed) C:\Users\johnsmith> DREAM3D.exe
(d3d_embed) C:\Users\johnsmith> PipelineRunner --pipeline /path/to/pipeline.json
```

The same environment must also be used for the Python bindings. The Python bindings are within the dream3d package and can be accessed like the following:

```lang-python
from dream3d import simpl
from dream3d import simplpy

dca = simpl.DataContainerArray()
result = simplpy.create_data_container(dca, data_container_name='foo')
```
