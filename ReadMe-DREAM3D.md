# DREAM.3D  #

DREAM.3D is an open source, cross-platform and modular software package that allows users to reconstruct, instantiate, quantify, mesh, handle and visualize multidimensional, multimodal data. DREAM.3D is built on top of the SIMPL and SIMPLView software projects and contains a filter suite designed for the materials scientist to utilize to reconstruct 3D  microstructures or synthetically generate microstructures. The core software package has many major functionalities, including:

+ Import of Electron Backscatter Diffraction (EBSD) data files (either TSL .ang or HKL .ctf) or images (.bmp, .jpeg, .tif or .png) as a 3D volume and export the data packed into a single HDF5 data file in order to reduce data size and clutter
+ Import of standard image files (TIFF, PNG, JPEG, BMP)
+ Alignment, cleaning, reconstruction, segmentation and analysis of imported data
+ Statistics, either synthetically created or from real data, can be used to generate a statistically equivelent material structure
+ Reconstructed and synthetic volumes can be surfaced meshed to allow export into FEM or other simulations.
+ Import/Export of data into/out of ASCII or Binary files

The above items represent only a subset of the specific capabilities of DREAM.3D.  In general, DREAM.3D can be considered a generic tool for processing multidimensional, multimodal data. The main [DREAM.3D web site](http://dream3d.bluequartz.net) has more information on the project, in addition to official binary distributions and example data files. DREAM.3D is built with a **Plugin** system that allows other institutions to build additional **Filters** using their own programming resources and intellectual property and still protect those rights under the BSD license.

## License ##
DREAM.3D is open-source and available under a BSD license. See the Resources/DREAM3D/DREAM3DLicense.txt file that comes with the project for more information.

## Building DREAM.3D ##
For information on building DREAM.3D and all its necessary dependencies, see the Documentation/ReferenceManual/Developer/ directory.

Current build status for the build farm is located at [our CDash site.](http://my.cdash.org/index.php?project=DREAM3D)

## Nightly Builds ##
Nightly snapshot builds are available for download [here](http://dream3d.bluequartz.net/binaries/experimental/). These binaries are built every night from the head of the develop branch, and may have bugs or other instabilities.

## Getting Help ##
Please sign up for the [DREAM.3D Users Google group](https://groups.google.com/forum/?hl=en#!forum/dream3d-users). It's the fastest way to ask your DREAM.3D questions, get help with using DREAM.3D, and connect with both developers and the community at large.  If you are interested in contributing to the DREAM.3D project, also sign up to the <a href="https://groups.google.com/forum/?hl=en#!forum/dream3d-developers">DREAM.3D Developers Google group</a> to get help with compiling and adding to DREAM.3D. If you find an error with DREAM.3D or want to request a new feature, please [open up an issue](https://github.com/dream3d/DREAM3D/issues).

## Contributing to DREAM.3D ##
Please reference the Contributing file for more information on contributing to the DREAM.3D project.

## Funding Sources ##

Major portions of DREAM.3D were written under US Air Force (AFRL) Contracts:
+ FA8650-07-D-5800
+ FA8650-10-D-5210
+ FA8650-15-D-5231

and Naval Research Labs (NRL) Contract:
+ N00173-07-C-2068.

# Many others have contributed to the code including the following (non-exhaustive) list: #

## Major Contributing Institutions ##

+ BlueQuartz Software (http://www.bluequartz.net)
+ US Air Force Research Laboratory (Materials Directorate)
+ Carnegie Mellon University, Department of Materials Science and Engineering
+ US Naval Research Laboratory
+ The Ohio State University

## List of Code (C) and Vision (V) Contributors  ###

### Air Force Research Laboratory (AFRL) ###

+ Dr. Michael A. Groeber (V, C)
+ Dr. Adam Pilchak (V, C)
+ Dr. Michael Uchic (V)
+ Dr. Chris Woodward (V)

### BlueQuartz Software ###

+ Ms. Sally Connell (C)
+ Dr. Dennis Dimiduk (V)
+ Dr. Sean P. Donegan (V, C)
+ Mr. Michael A. Jackson (V, C)
+ Mr. Joseph B. Kleingers (C)
+ Dr. Megna Shah (V, C)


### Ohio State University/Johns Hopkins University ###

+ Dr. Somnath Ghosh (V)

### Carnegie Mellon University ###

+ Dr. Abhijeet Brahme (C)
+ Dr. Patrick Callahan (C)
+ Dr. Lisa Chan (C)
+ Dr. Marc De Graef (C)
+ Dr. Joe Fridy (C)
+ Dr. Sukbin Lee (C)
+ Dr. Greg Rohrer (C)
+ Dr. Tony Rollett (V, C)
+ Dr. David Saylor (C)
+ Dr. Stephen Sintay (C)
+ Dr. Shlomo Ta'asan (C)
+ Dr. Joseph C. Tucker (V, C)


### Naval Research Laboratory (NRL) ###

+ Dr. Dave Rowenhorst (V, C)

### Other Institutions ###

+ Dr. Megna Shah (V, C)
+ Dr. Krzysztof Glowinski (C) (Institute of Metallurgy and Materials Science PAS, Krakow, Poland)
+ Mr. William Lenthe (V, C) (University of California, Santa Barbara)
+ Dr. Adam Morawiec (V) (Institute of Metallurgy and Materials Science PAS, Krakow, Poland)
+ Mr. Matthew Priddy (C) (Georgia Tech University)
+ Mr. Jarrell Waggoner (C) (Univ. South Carolina)

