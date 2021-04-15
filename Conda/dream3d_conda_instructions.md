# Building a DREAM3D Anaconda Package #

1. Install [Visual Studio](https://visualstudio.microsoft.com/downloads/) (Windows)
2. Install [Anaconda](https://docs.anaconda.com/anaconda/install/)
    

3. Setup [conda-forge](https://conda-forge.org/) channel (requires conda >= 4.6)
    ```
    conda config --add channels conda-forge
    conda config --set channel_priority strict
    ```
4. Create a new conda environment
    ```
    conda create -n dream3d python=3.8
    conda activate dream3d
    ```
4. Install conda-build
    ```
    conda install conda-build
    ```
5. Clone DREAM3D development repos into a new directory of your choice
    ```
    git clone https://github.com/BlueQuartzSoftware/DREAM3D
    git clone https://github.com/BlueQuartzSoftware/SIMPL
    git clone https://github.com/BlueQuartzSoftware/SIMPLView
    git clone https://github.com/BlueQuartzSoftware/EbsdLib
    git clone https://github.com/BlueQuartzSoftware/H5Support
    git clone https://github.com/BlueQuartzSoftware/CMP
    git clone https://github.com/dream3d/DREAM3D_Data
    mkdir DREAM3D_Plugins
    cd DREAM3D_Plugins
    git clone https://github.com/BlueQuartzSoftware/SimulationIO
    git clone https://github.com/BlueQuartzSoftware/ITKImageProcessing
    git clone https://github.com/dream3d/DREAM3DReview
    git clone https://github.com/dream3d/UCSBUtilities
    ```
6. Unzip the following in DREAM3D_Data
    - DREAM3D_Data/Data/Image.tar.gz
    - DREAM3D_Data/Data/SmallN100.tar.gz
    - DREAM3D_Data/Data/T12-MAI-2010.tar.gz
7. Use conda build on DREAM3D
    
    The build takes around 15 min to complete on a 12 core machine.
    
    The initially "solving environment" step may take a while in particular, and it does not have a progress indicator.
    ```
    conda build /path/to/DREAM3D/Conda
    ```
8. Install locally build DREAM3D package into the current environment

    Make sure the Python verion of the environment matches the version used to build with DREAM3D.
    ```
    conda install -c local dream3d
    ```

# Running DREAM3D from Anaconda #

DREAM3D, and related applications, must be run from within the conda environment.

i.e.
```
conda activate dream3d
DREAM3D
PipelineRunner --pipeline /path/to/pipeline.json
```

The same environment must also be used for the Python bindings. The Python bindings are within the dream3d package and can be accessed like the following:

```
from dream3d import simpl
from dream3d import simplpy

dca = simpl.DataContainerArray()
result = simplpy.create_data_container(dca, data_container_name='foo')
```

# Writing DREAM3D filters in Python #

Inside SIMPL/Source/SIMPLib/Python, there are two examples of filters written in Python, `initialize_data.py` and `ExampleFilter.py`.

Filters must derive from the abstract base class `dream3d.Filter` and implement its methods similiary to C++ filters. Then inside the Python file, a global variable `filters` must exist which should be a list of types where each type is a filter.

To use Python filters in DREAM3D or PipelineRunner, create an environment variable `SIMPL_PYTHONPATH`. This variable should contain a list(platform delimited i.e. ":" on Unix and ";" on Windows) of directories to load Python filters from. All Python files should be of the above format.

If use the DREAM3D GUI, Python filters can be reloaded at runtime. DREAM3D will clear all existing Python filters and attempt to reload them from directories in `SIMPL_PYTHONPATH`. Currently loaded pipelines with Python filters will be preserved if the relevant filters still exist in those directories.
