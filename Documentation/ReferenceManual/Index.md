![](UsingDREAM3D/Images/DREAM3D.png)

#DREAM.3D #

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
+ Over 350 filters to process data
+ Integration with [http://www.itk.org](http://www.itk.org)
+ Use [ParaView](http://www.paraview.org) to visualize raw and processed data

The above items represent only a subset of the specific capabilities of DREAM.3D.  In general, DREAM.3D can be considered a generic tool for processing multidimensional, multimodal data. The main [DREAM.3D web site](http://dream3d.bluequartz.net) has more information on the project, in addition to official binary distributions and example data files. DREAM.3D is built with a **Plugin** system that allows other institutions to build additional **Filters** using their own programming resources and intellectual property and still protect those rights under the BSD license.


## WebSite ##

[http://dream3d.bluequartz.net](http://dream3d.bluequartz.net)

## License ##

See the Copying.md file for CopyRight & license


## History of DREAM.3D ##
The history of DREAM.3D is rooted in two parallel research efforts:

+ The first was the graduate research project of Michael Groeber, under the advisement of Dr. Somnath Ghosh, at The Ohio State University (2003-2007).  The key developments under this project were the creation of many of DREAM.3D's current filters for reconstructing serial section experimental datasets, quantifying microstructural features in 3D and generating statistically equivalent microstructures.
+ The second research effort was carried out by the research group of Dr. Tony Rollett at Carnegie Mellon University over a number of graduate student projects (see names below) and years (~2000-2011). The key developments under these projects were the development of a synthetic microstructure builder, microstructure clean-up tools, surface mesh generation and quantification tools for microstructural features.

The tools developed under these efforts generally remained as command line executables with limited portability due to lack of documentation and flexibility.  In an effort to extend the capabilites of these tools to the larger materials community, Dr. Chris Woodward of the Air Force Research Laboratory (AFRL) funded Mike Jackson of BlueQuartz Software to create Graphical User Interfaces (GUIs), common data structures and documentation for the tools developed in the research effort of Groeber and Ghosh (2009-2011). After the initial phase of development (2011), Jackson and Groeber approached Rollett, Lee, Sintay, Chan and Tucker and proposed the unification of the parallel efforts.  The Rollett group accepted the proposal and graciously made available all of their algorithms and source code.  It was at this point that DREAM.3D was named and the vision for the software was truly solidified.

Over the next couple of years (2011-2013), additional researchers contributed code to extend the capabilities of DREAM.3D.  Specifically, the grain boundary-centric tools of Dr. Dave Rowenhorst (Naval Research Laboratory) and Dr. Greg Rohrer (Carnegie Mellon University) greatly augmented the functionality of the package.

By 2013, Jackson and Groeber had fully documented the tools, created a Software Developer's Kit (SDK) and generated tools for adding plugins to the package, making the software package readily usable and extensible. As the community surrounding DREAM.3D continued to grow, Jackson formed BlueQuartz Software into an LLC to better provide professional support and allow for additional growth.  In 2014, Dr. Sean Donegan joined BlueQuartz Software from the Rollett group to provide another full-time developer with materials expertise.  In late 2014, BlueQuartz also added  Joseph Kleingers and Sally Connell as full-time developers with computer science backgrounds.

During the growth of BlueQuartz Software through 2014, the core [data structure](@ref datastructure) of DREAM.3D was redesigned to allow for greater user flexibility.  This change also allowed the tool to expand outside of microstructural analysis to include most areas of materials research, and ultimately to multidimensional data analysis in general. These efforts spurred the release of the current version of DREAM.3D, version 6, which represents a great step forward in the DREAM.3D experience.  

## Building DREAM.3D ##

For information on building DREAM.3D and all its necessary dependencies, see the Documentation/ReferenceManual/Developer/ directory. )

Current build status for the build farm is located at [our CDash site.](http://my.cdash.org/index.php?project=DREAM3D)

## Nightly Builds ##

Nightly snapshot builds are available for download [here](http://dream3d.bluequartz.net/binaries/experimental/). These binaries are built every night from the head of the develop branch, and may have bugs or other instabilities.

## Getting Help ##

Please sign up for the [DREAM.3D Users Google group](https://groups.google.com/forum/?hl=en#!forum/dream3d-users). It's the fastest way to ask your DREAM.3D questions, get help with using DREAM.3D, and connect with both developers and the community at large.  If you are interested in contributing to the DREAM.3D project, also sign up to the <a href="https://groups.google.com/forum/?hl=en#!forum/dream3d-developers">DREAM.3D Developers Google group</a> to get help with compiling and adding to DREAM.3D. If you find an error with DREAM.3D or want to request a new feature, please [open up an issue](https://github.com/dream3d/DREAM3D/issues).

## Contributing to DREAM.3D ##

Please reference the Contributing file for more information on contributing to the DREAM.3D project.

## Acknowledgements ##

[See the file for contributors to DREAM.3D](UsingDREAM3D/Acknowledgements.html)

