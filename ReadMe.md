![](https://github.com/BlueQuartzSoftware/DREAM3D/raw/develop/Resources/OpenSourceEdition/splash/branded_splash.png)

# DREAM.3D  #

DREAM.3D is an open source, cross-platform and modular software package that allows users to reconstruct, instantiate, quantify, mesh, handle and visualize multidimensional, multimodal data. DREAM.3D is built on top of the SIMPL and SIMPLView software projects and contains a filter suite designed for the materials scientist to utilize to reconstruct 3D  microstructures or synthetically generate microstructures. The core software package has many major functionalities, including:

+ Import ASCII Data
+ Import EBSD Vendor Data (.ang, .ctf, .h5)
+ Import of standard image files (TIFF, PNG, JPEG, BMP)
+ Archive Raw and processed data to HDF5 files
+ Alignment, cleaning, reconstruction, segmentation and analysis of imported data
+ Statistics, either synthetically created or from real data, can be used to generate a statistically equivelent material structure
+ Reconstructed and synthetic volumes can be surfaced meshed to allow export into FEM or other simulations.
+ Import/Export of data into/out of ASCII or Binary files
+ Export Surface Mesh as STL Files
+ Over 200 filters to process data
+ Integration with [http://www.itk.org](http://www.itk.org)
+ Use [ParaView](http://www.paraview.org) to visualize raw and processed data

The above items represent only a subset of the specific capabilities of DREAM.3D.  In general, DREAM.3D can be considered a generic tool for processing multidimensional, multimodal data. The main [DREAM.3D web site](http://dream3d.bluequartz.net) has more information on the project, in addition to official binary distributions and example data files. DREAM.3D is built with a **Plugin** system that allows other institutions to build additional **Filters** using their own programming resources and intellectual property and still protect those rights under the BSD license.


## WebSite ##

[http://dream3d.bluequartz.net](http://dream3d.bluequartz.net)

## License ##

See the Copying.md file for CopyRight & license


## Building DREAM.3D ##

For information on building DREAM.3D and all its necessary dependencies, see the Documentation/ReferenceManual/Developer/ directory. [View it here on the GitHub website](https://github.com/BlueQuartzSoftware/DREAM3D/tree/develop/Documentation/ReferenceManual/Developer)

Current build status for the build farm is located at [our CDash site.](http://my.cdash.org/index.php?project=DREAM3D)

## Nightly Builds ##

Nightly snapshot builds are available for download [here](http://dream3d.bluequartz.net/binaries/experimental/). These binaries are built every night from the head of the develop branch, and may have bugs or other instabilities.

## Getting Help ##

Please sign up for the [DREAM.3D Users Google group](https://groups.google.com/forum/?hl=en#!forum/dream3d-users). It's the fastest way to ask your DREAM.3D questions, get help with using DREAM.3D, and connect with both developers and the community at large.  If you are interested in contributing to the DREAM.3D project, also sign up to the <a href="https://groups.google.com/forum/?hl=en#!forum/dream3d-developers">DREAM.3D Developers Google group</a> to get help with compiling and adding to DREAM.3D. If you find an error with DREAM.3D or want to request a new feature, please [open up an issue](https://github.com/dream3d/DREAM3D/issues).

## Contributing to DREAM.3D ##

Please reference the Contributing file for more information on contributing to the DREAM.3D project.

## Acknowledgments ##

See the file for contributors to DREAM.3D
