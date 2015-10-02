# DREAM.3D  #

DREAM.3D is an open source, cross-platform and modular software package that allows users to reconstruct, instantiate, quantify, mesh, handle and visualize multidimensional, multimodal data. The core software package has many major functionalities, including:

+ Import a series of Electron Backscatter Diffraction (EBSD) data files (either TSL .ang or HKL .ctf) or images (.bmp, .jpeg, .tif or .png) as a 3D volume and export the data packed into a single HDF5 data file in order to reduce data size and clutter
+ Alignment, cleaning, reconstruction, segmentation and analysis of imported data
+ Statistics, either synthetically created or from real data, can be used to generate a statistically equivelent material structure
+ Reconstructed and synthetic volumes can be surfaced meshed to allow export into FEM or other simulations.

The above items represent only a subset of the specific capabilities of DREAM.3D.  In general, DREAM.3D can be considered a generic tool for processing multidimensional, multimodal data. The main [DREAM.3D web site](http://dream3d.bluequartz.net) has more information on the project, in addition to official binary distributions and example data files. DREAM.3D is built with a **Plugin** system that allows other institutions to build additional **Filters** using their own programming resources and intellectual property and still protect those rights under the BSD license.

## License ##
DREAM.3D is open-source and available under a BSD license. See the Resources/DREAM3D/DREAM3DLicense.txt file that comes with the project for more information.

## Building DREAM.3D ##
For information on building DREAM.3D and all its necessary dependencies, see the Documentation/Developer/ directory.

## Getting Help ##
Please consider signing up for the <a href="https://groups.google.com/forum/?hl=en#!forum/dream3d-users">DREAM.3D Users Google group</a>! It's the fastest way to ask your DREAM.3D questions, get help with using DREAM.3D, and connect with both developers and the community at large.  If you are interested in contributing to the DREAM.3D project, also sign up to the <a href="https://groups.google.com/forum/?hl=en#!forum/dream3d-developers">DREAM.3D Developers Google group</a> to get help with compiling and adding to DREAM.3D. If you find an error with DREAM.3D or want to request a new feature, please [open up an issue](https://github.com/dream3d/DREAM3D/issues).

## Contributing to DREAM.3D ##
The DREAM.3D project utilizes the [fork and pull](https://help.github.com/articles/using-pull-requests/) model for development, and we welcome pull requests! If you have a **Plugin** you would like to see distributed with the DREAM.3D binary, but do not want it merged into the main DREAM.3D repository, you can request to have thate **Plugin** become _curated_. The DREAM.3D development team will then store that **Plugin's** source at our parent [GitHub page](https://github.com/dream3d/). This allows developers to maintain control over their source code while still having their work distributed with the main DREAM.3D application to the public. Please contact the DREAM.3D development team if you are interested in curated **Plugins**.

## Nightly Builds ##
Nightly snapshot builds are available for download [here](http://dream3d.bluequartz.net/binaries/experimental/). These binaries are built every night from the head of the develop branch, and may have bugs or other instabilities.


## Funding Sources ##
Parts of DREAM.3D were written under US Air Force (AFRL) Contract FA8650-07-D-5800 and Naval Research Labs (NRL) Contract N00173-07-C-2068. Many others have contributed to the code including the following (non-exhaustive) list:

## Major Contributing Institutions ##

+ US Air Force Research Laboratory (Materials Directorate)
+ BlueQuartz Software (http://www.bluequartz.net)
+ Carnegie Mellon University, College of Materials Science and Engineering
+ US Naval Research Laboratory
+ The Ohio State University

## List of Code Contributors (CC) and Vision Contributors (VC) ###

### Air Force Research Laboratory (AFRL) ###


+ Dr. Michael A. Groeber (VC, CC)
+ Dr. Adam Pilchak (VC, CC)
+ Dr. Megna Shah (VC, CC)
+ Dr. Michael Uchic (VC)
+ Dr. Chris Woodward (VC)

### BlueQuartz Software ###


+ Mr. Michael A. Jackson (VC, CC)
+ Dr. Sean P. Donegan (VC, CC)
+ Dr. Dennis Dimiduk (VC)
+ Mr. Joseph B. Kleingers (CC)
+ Ms. Sally Connell
+ Dr. Megna Shah (VC, CC)


### Ohio State University/Johns Hopkins University ###


+ Dr. Somnath Ghosh (VC)

### Carnegie Mellon University ###


+ Dr. Tony Rollett (VC, CC)
+ Dr. Greg Rohrer (CC)
+ Dr. Marc De Graef (CC)
+ Dr. Sukbin Lee (CC)
+ Dr. Joseph C. Tucker (VC, CC)
+ Dr. Lisa Chan (CC)
+ Dr. Stephen Sintay (CC)
+ Dr. Abhijeet Brahme (CC)
+ Dr. David Saylor (CC)
+ Dr. Joe Fridy (CC)
+ Dr. Patrick Callahan (CC)
+ Dr. Shlomo Taasan (CC)

### Naval Research Laboratory (NRL) ###


+ Dr. Dave Rowenhorst (VC, CC)

### Other Institutions ###

+ Mr. William Lenthe (VC, CC) (University of California, Santa Barbara)
+ Mr. Matthew Priddy (CC) (Georgia Tech University)
+ Mr. Jarrell Waggoner (CC) (Univ. South Carolina)


