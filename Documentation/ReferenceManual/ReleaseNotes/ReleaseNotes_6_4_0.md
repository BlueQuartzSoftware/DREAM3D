Version 6.4.0 Release Notes {#releasenotes_6_4_0}
===========

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/bluequartzsoftware/DREAM3D/issues).

## Getting Help ##

The developers of DREAM.3D maintain a pair of Google Groups for discussions on the use of DREAM.3D and the development of filters or plugins for DREAM.3D. If you have a question those are the best places to ask and get help from the community.

[DREAM.3D Users List](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[DREAM.3D Developers List](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.4.x ##


### Version 6.4.0 ###

+ User interface changes which should improve usability.
+ User interface changes to the pipeline view that shows more information to the user before and during execution of a pipeline
+ Add color to each of the filters in the pipeline view that denotes which major group the filter is categorized as.

+ BUG: Fixed issue when reading .ang file the last row of data would not be parsed from file
+ FEATURE: Allow user to re-edit the ASCI Import Wizard settings after clicking the "Done" button. (Issue 599)
+ Added Kahan summation to FindShapes filter
+ Fix crashing bug when Manhattan distance is OFF in the Find Euclidean Distance Map filter
+ Moving FeatureDataCSVWriter, ReadASCIIData, WriteASCIIData and WriteTriangleGeometry filters from IO to CoreFilters
+ Fix Generate Ensemble Statistics where the crystallographic variables were being controlled by the Radial Distribution checkbox. 
+ Any filter that can "Read" or "Write" data are now named "Import" and "Export" except in the case of reading and writing DREAM.3D files
+ InsertPrecipitatePhases now returns a warning instead of an error when no precipitate phases are found. updates #394 
+ Wizard for ImportASCIIData has been updated with new capabilities and clearer error messages
+ The entire UI for StatsGenerator filter has been redesigned.
+ New Filter MultithresholdObjects2 has a brand new interface allowing the user to perform complex boolean operations on data
+ Find Misorientation Filter: Use NaN for values if the misorientation/misalignment can not be calculated instead of -100.0
+ Fixing crash bug when index out of bounds happens because two features both with an unknown crystal structure (999) is used for a calculation. In this case do NOT calculate any value.
+ Fixing bug in filters that depended on the proper bounding box being calculated. FindShapes filter was updates with refactoring FindFeatureCentroids was fixed to account for the origin when computing the centroids of each feature. 
+ On Mac OS X, DREAM.3D is now a single .app package.
+ Add filter to compute the Central Moments for 2D slices of data (Moment Invariant 2D)
+ Add filter to compute the min XYZ and max XYZ pixel coordinates for each feature.
+ Update ‘SpaceGroupOps’ to ‘LaueOps’
+ Enable search and MathJax in Doxygen Allows for searchable help and well-rendered Latex equations.
+ EBSD Standard Triangle codes for all Laue classes except Cubic 3m Tetrahedral
+ Rename ‘FlattenImage’ to ‘ConvertColorToGrayScale’ and add additional options for the conversion algorithm
+ Adding "Use 1-Based Values" boolean option to EMMPM filter.
+ Add filters that compute morphological statisics of features in TriangleGeoms
+ Fix crashing when selecting ShapeTypes through the ShapeTypes Widget
+ Updating the Ph and Dx readers to properly size their ImageGeometry and Cell AttributeMatrix during preflight
+ Fix issue when reading .ang files where the last row was not properly parsed and stored into the internal arrays
+ Add filter that computes the enclosed volume of surface meshed features
+ Updating the Generate Precipitate StatsData with Radial Distribution Function (RDF) codes
+ Fix issue where prebuilt pipeline did not have proper default values.
+ Extend QuickSurfaceMesh to function on RectGrid geometries 
+ Creating stand-alone filter that can generate a PrimaryPhase from the base feature size statistics
+ Extend the FindSizes filter to operate on any geometry
+ Fixing a bug where the menu button would pop up its menu in the wrong place due to an incorrect point conversion
+ Add capability for manual initialization of mean and standard deviation values within the DREAM.3D filter implementation of the EMMPM library
+ FeatureDataCSVWriter now allows users a choice between using a comma, semicolon, colon, or tab as delimiters.
+ Fixing memory leak in H5AngReader.
+ Add images to illustrate the relationship between the Dimension and Resolution.
+ Fixing internationalization issues with decimal separator values.
+ Fixing bug where the Neighbor Orientation Correlation filter would not paste the Misorientation Tolerance value correctly after being executed once.
+ Fix reading and writing of DX files so that the data is read/written in ZYX ordering


Update the StatsGenerator UI to allow the user easier access to input fields. This version updates the StatsGenerator UI with the following:
+ Create a stand alone FeatureSizeDistWidget that handles all the interactions
with the ESD Size Distribution.
+ Create class hierarchy for PrimaryPhaseWidget and PrecipitatePhaseWidget
in order to reused code where possible.
+ Update the documentation with new and additional images and clarifications.
updates #571
+ Allow user to set a specific name for each phase. updates #576
+ Remove code that is no longer used/required.
+ Format the various distribution plots with new dark color scheme
+ Add bins to the Feature Size Probability Density Function plot
+ Allow user to input a true ESD that then calculates the value of Mu
+ Mousing over ESD PDF plot shows the value under the mouse pointer.

### Developer Notes ###

Releases are now built against the following libraries:

+ Qt 5.6.2
+ HDF5 1.8.16
+ Boost 1.60.0
+ Qwt 6.1.3
+ Eigen 3.2.9
+ ITK 4.11.1
+ TBB tbb44_20160526oss

