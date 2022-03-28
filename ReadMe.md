# About DREAM.3D #

![DREAM3D Logo](https://github.com/BlueQuartzSoftware/DREAM3D/raw/develop/Resources/OpenSourceEdition/icons/DREAM3D%20(PNG)/128x128.png)

DREAM.3D is an open source, cross-platform and modular software package that allows users to reconstruct, instantiate, quantify, mesh, handle and visualize multidimensional, multimodal data. DREAM.3D is built on top of the SIMPL and SIMPLView software projects and contains a filter suite designed for the materials scientist to utilize to reconstruct 3D  microstructures or synthetically generate microstructures. The core software package has many major functionalities, including:

## Precompiled Binaries for Windows/MacOS/Linux ##

Please download a precompiled binary for your platform from [dream3d.bluequartz.net](http://dream3d.bluequartz.net)

## Anaconda Python Distribution ##

There is an anaconda distribution available for DREAM.3D. This gives python developers/users the ability to call every DREAM.3D
filter directly from python.

```(lang-console)
conda config --add channels conda-forge
conda create  -n dream3d python=3.7 numpy imageio
conda activate dream3d
conda install -c http://dream3d.bluequartz.net/binaries/conda dream3d-conda
```
The API docs are located at [http://www.dream3d.io/python_docs/html/](http://www.dream3d.io/python_docs/html/)

## General Features ##

+ Over 350 filters to process data
+ Import/Export CSV, text and binary data
+ Import/Export of OEM EBSD Data (.ang, .ctf, .h5)
+ Import/Export of Grayscale and Color Images (TIFF, PNG, JPEG, BMP, etc... )
+ Export STL Files
+ Import Data from HDF5 files
+ Save files to HDF5 files
+ Process data with many different filters
+ Compute many microstructure statistics (grain size, average orientation, etc..)
+ Generate synthetic microstructures of many types and multiple phases.
+ Export Surface Mesh as STL Files
+ Integration with [http://www.itk.org](http://www.itk.org)

## EBSD Data Processing ##

+ Import and processing of data from EDAX/Oxford Instruments/Bruker Nano.
+ IPF Color Map, Pole Figure and other visualization techiques
+ Cleanup EBSD scans, Segment Grains, compute microstructure statistics
+ Alignment, cleaning, reconstruction, segmentation and analysis of imported data

## Synthetic Microstructure Generation ##

+ Generate a synthetic microstructure from experimental data or from user specified data.
+ Export your microstructure models to FEM solvers (Abaqus, Ansys, OnScale, Simmetrix... )

## Data Visualization ##

+ Use [ParaView](http://www.paraview.org) to visualize raw and processed data
+ Use Avizo to visualize data.

The above items represent only a subset of the specific capabilities of DREAM.3D.  In general, DREAM.3D can be considered a generic tool for processing multidimensional, multimodal data. The main [DREAM.3D web site](http://dream3d.bluequartz.net) has more information on the project, in addition to [official binary distributions](http://dream3d.bluequartz.net/?page_id=32) and [example data files](http://www.github.com/dream3d/DREAM3D_Data). 

DREAM.3D is built with a **Plugin** system that allows other institutions to build additional **Filters** using their own programming resources and intellectual property and still protect those rights under a license of their selection.

## Documentation ##

The documentation for the current release is posted at [http://www.dream3d.io](http://www.dream3d.io). This is the same documentation that is available from within the DREAM.3D application, just posted online.

## License ##

See the Copying.md file for CopyRight & license

## Building DREAM.3D ##

If you would like to create new features/functionality/filters for your own use or to contribute back to the community, please go to the [DREAM3DSuperbuild](https://github.com/BlueQuartzSoftware/DREAM3DSuperbuild/tree/develop)

Current build status for the build farm is located at [the CDash site.](http://my.cdash.org/index.php?project=DREAM3D)

## Nightly Builds ##

Nightly snapshot builds are available for download [here](http://dream3d.bluequartz.net/binaries/experimental/). These binaries are built every night from the head of the develop branch, and may have bugs or other instabilities.

## Getting Help ##

Please sign up for the [DREAM.3D Users Google group](https://groups.google.com/forum/?hl=en#!forum/dream3d-users). It's the fastest way to ask your DREAM.3D questions, get help with using DREAM.3D, and connect with both developers and the community at large.  If you are interested in contributing to the DREAM.3D project, also sign up to the [DREAM.3D developers Google group](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers) to get help with compiling and adding to DREAM.3D.

If you find an error with DREAM.3D or want to request a new feature, please [open up an issue](https://github.com/bluequartzsoftware/DREAM3D/issues).

## Contributing to DREAM.3D ##

Please reference the Contributing file for more information on contributing to the DREAM.3D project.

## Acknowledgments ##

DREAM.3D has been developed under several United States Department of Defense contracts. The DoD has graciously allowed the bulk of DREAM.3D to be open-sourced. In particular the Air Force Research Laboratory was the principal funding agency through out most of the development. Code contributions from academia including Carnegie Mellon University were also instrumental in creating DREAM.3D. The full list of contributors can be found [here](https://github.com/BlueQuartzSoftware/DREAM3D/raw/develop/Documentation/ReferenceManual/5_ReleaseNotes/Acknowledgements.md)
