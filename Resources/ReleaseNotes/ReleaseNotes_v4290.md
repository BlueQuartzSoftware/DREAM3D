## Important Changes in Version 4.2 ##

### TSL Reference Frame Changes ###

Versions prior to 4.2 did not correctly set the sample and crystal reference frame transformations when importing data into an .h5ebsd file. This bug has been corrected but due to changes within the .h5ebsd file the user is *strongly* encouraged to reimport any .ang data files an rerun any pipelines that depend on that data. More information can be found in the "Import Orientation Data" documentation.

### Changes to Internal Quaternion Structure ###
For historical compatibility in versions prior to 4.2 the internal representation of a quaternion was a 5 component array for each quaternion. This compatibility is no longer needed and the internal structure has been changed to a standard (X, Y, Z, W) style quaternion. This will create issues when loading .dream3d files that were created with versions prior to 4.2 into DREAM3D versions 4.2 and beyond. In order to alleviate migration issues a new filter in the "Update Cell Quaternions" located in the "Generic" section of the filter library will convert existing 5 component quaternions to the newer 4 component layout. If the user would like to simply update their .dream3d file the pipeline to accomplish this would be:

+ Read DREAM3D File (Check EVERY array to read)
+ Update Cell Quaternions
+ Write DREAM3D File (Check EVERY array to write)

**Note: This pipeline will OVERWRITE the existing DREAM3D File if both the input and output files are the same file.**

#### Version 4.2.90 Bugs & Features Fixed ###
+ Fixed bug where Quaternion::ElementWiseAbsd was casting to integers due to use of 'abs' instead of 'fabs'
+ Fixed bug in FindSurfaceVoxelFraction where the types used in the macros were not consistent.
