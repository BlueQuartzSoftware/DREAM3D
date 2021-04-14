# Building a DREAM3D Anaconda Package #

1. Install [Visual Studio](https://visualstudio.microsoft.com/downloads/) (Windows)
2. Install [Anaconda](https://docs.anaconda.com/anaconda/install/)
3. Setup [conda-forge](https://conda-forge.org/) channel
    ```
    conda config --add channels conda-forge
    conda config --set channel_priority strict
    ```
4. Install conda-build
    ```
    conda install conda-build
    ```
5. Clone DREAM3D development repos
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
    ```
    conda build /path/to/DREAM3D/Conda
    ```
8. Install locally build DREAM3D package
    ```
    conda install -c local dream3d
    ```
