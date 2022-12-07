DREAM.3D consists of data analysis tools (Filters) that allow for the construction of customized workflows (Pipelines) to analyze data. DREAM.3D provides a flexible and extensible data structure that eases data transport between collaborators by storing data in a non-proprietary format.

DREAM.3D makes the reconstruction of 3D data simple and straight forward. The development of additional features is ongoing and the DREAM.3D development team welcomes your feedback whether you are a first time user or seasoned user.

All source codes are publicly available through the [GitHub](http://www.github.com/bluequartzsoftware/DREAM3D) repository and also has the official bug tracker.

DREAM.3D is completely open source and free for anyone to use whether that is in a commercial, academic or research setting. We encourage every one to give it a try and provide feedback about your experience.

## Features of DREAM.3D ##

+ 3D Reconstruction of EBSD data from EDAX (.ang), Oxford (.ctf) and Bruker (.ctf) data files. The reconstructions can utilize an array of alignment, cleaning, segmentation algorithms and coloring algorithms.
+ Synthetic microstructures can be created using a set of automatically generated statistics or your own statistics.
+ The reconstructed volumes can be exported as industry standard STL files, ParaView files (.xdmf), Abaqus (.inp). DREAM.3D stores all data as HDF5 files by default.
+ Many algorithms are available to extract various statistics about your data
+ Over 100 filters from the image processing library ITK

## Prebuilt Binaries ##

The current version is 6.5.167 and is available in prebuilt binaries for MacOS, Windows and Linux operating systems:

| Operating System | Notes |
|------------------|----------------------|
| Signed [MacOS - DREAM3D-6.5.167-OSX-x86_64.dmg](http://dream3d.bluequartz.net/binaries/DREAM3D-6.5.167-OSX-x86_64.dmg) | MacOS 10.15 and greater required, including macOS 11.0. Download is a Disk Image |
| Unsigned [MacOS - DREAM3D-6.5.167-OSX-x86_64.zip](http://dream3d.bluequartz.net/binaries/DREAM3D-6.5.167-OSX-x86_64.zip) | MacOS 10.15 and greater required, including macOS 11.0. Download is a Zip file |
| Signed [MacOS - DREAM3D-6.5.167-OSX-arm64.dmg](http://dream3d.bluequartz.net/binaries/DREAM3D-6.5.167-OSX-arm64.dmg) | Apple M1 Arm build. Download is a DMG File |
| Unsigned [MacOS - DREAM3D-6.5.167-OSX-arm64.dmg](http://dream3d.bluequartz.net/binaries/DREAM3D-6.5.167-OSX-arm64.dmg) | Apple M1 Arm build. Download is a ZIP File |
| [Windows - DREAM3D-6.5.167-Win64.zip](http://dream3d.bluequartz.net/binaries/DREAM3D-6.5.167-Win64.zip) | Windows version 8,10,11 |
| [Linux - DREAM3D-6.5.167-Linux-x86_64.tar.gz](http://dream3d.bluequartz.net/binaries/DREAM3D-6.5.167-Linux-x86_64.tar.gz) | Ubuntu 18.04 or Equivelant. Self contained tar archive.  |


## Nightly Builds ##

The nightly version is 6.6.xxx and is available in prebuilt binaries for MacOS, Windows and Linux operating systems:

[Start Here and look for todays date or the latest available date.](http://dream3d.bluequartz.net/binaries/experimental)

## Python Anaconda Distribution ##

```lang-console
(base) C:\Users\johnsmith> conda config --add channels conda-forge
(base) C:\Users\johnsmith> conda config --set channel_priority strict
(base) C:\Users\johnsmith> conda create -n pyD3D python=3.8
(base) C:\Users\johnsmith> conda activate pyD3D
(pyD3D) C:\Users\johnsmith> 
(pyD3D) C:\Users\johnsmith>conda install -c http://dream3d.bluequartz.net/binaries/conda dream3d-conda
(pyD3D) C:\Users\johnsmith>conda install libharu 2.0.0
```

## Documentation ##

+ The HTML documentation is located [here](http://www.dream3d.io) and also available within the application itself.
+ BlueQuartz maintains a [YouTube Channel that has some instructional videos](https://www.youtube.com/channel/UCjeF8pFMzET5ZN3vsBHATpg)

## Discussion Group ##

If you are looking for help using DREAM.3D there is an [free discussion group on Google](https://groups.google.com/g/dream3d-users)

## Citing DREAM.3D in Publications ##

Users wishing to cite DREAM.3D in their research publications are referred to [https://link.springer.com/article/10.1186/2193-9772-3-5](https://link.springer.com/article/10.1186/2193-9772-3-5) for the proper citations.


![images/DREAM3D_Logo_Small.png](images/DREAM3D_Logo_Small.png)

+ Looking for the DREAM.3D Icon for a presentation? [This one is 2048 x 2048 pixels](images/DREAM3D_Logo.png)

