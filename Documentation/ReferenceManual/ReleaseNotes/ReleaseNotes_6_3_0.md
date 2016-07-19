Version 6.3.0 Release Notes {#releasenotes_6_3_0}
===========

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/DREAM3D/DREAM3D/issues).

## Getting Help ##

The developers of DREAM.3D maintain a pair of Google Groups for discussions on the use of DREAM.3D and the development of filters or plugins for DREAM.3D. If you have a question those are the best places to ask and get help from the community.

[DREAM.3D Users List](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[DREAM.3D Developers List](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.3 ##


### Version 6.3.29 ###

+ BUG: Fixed issues when DREAM.3D is run on a system with a European locale. Entering decimal numbers will not work due to a different between the use of "," and ".".
+ BUG: Fixed crashing bug when dragging filters in the pipeline.
+ ENH: Adding option to use Taubin Smoothing in the Laplacian surface meshing filter
+ ENH: Added new filter to calculate the 3D coordinates from a quaternion.
+ ENH: Enabling the Avizo writer filters.
+ BUG: Fixing issue where the Filter index group was not closed leading to corrupted Pipeline files if "Find Boundary Strengths" filter is used.
+ BUG: Each Plugin now has a unique version specific to itself.
+ BUG: Fixing issue where the ComparisonSelectionWidget would not refresh its list of DataContainers and AttributeMatrices after an upstream pipeline was changed.


### Version 6.3.0 ###

This is a major update and all users of DREAM.3D are encouraged to update to this version.

### Major Changes ###
+ Created new "ToolBox" window that contains the *Filter List*, *Filter Library* and *Bookmarks* tabs from earlier versions
+ Cut/Copy/Paste of Filters within DREAM.3D pipelines and between DREAM.3D documents.
+ Added OrientationLib which uses a unified source code for calculating crystallographic angle transforms.

### DREAM.3D Changes ###

+ Feature: Add ability to write grayscale images instead of only RGB/RGBA images.
+ BUG: Fix issue where the ReadH5Ebsd filter widget was not being disabled during pipeline execution.
+ BUG: Fix issue with **Generate Ensemble Statistics** filter where extra parameters were added to a few of the LinkedBoolean widgets which would cause a crash when running the pipeline.
+ Fixing a bug that was causing DREAM3D to crash when quitting if the user launched the help dialog during the session.
+ Adding explanation to CTF readers why 30 degrees is added to phi2 angle.
+ BUG: Enforce that the packing grid has dimension of at least 1.
+ BUG: Fix incorrect computation of seed during segmentation. Thanks to W. Lenthe for the bug report.
+ BUG: Fixing an issue when reading a H5EBSD file where the Euler Angles are represented in Degrees AND the user selects to apply the 'Recommended Transformations'. The Euler Angles are silently treated as radians for some internal calculations that transform from Euler angles to an Orientation Matrix and back to Euler Angles.
+ Feature: Added filter **Find GBCD (Metric Based)**. Thanks to Krzysztof Glowinski for donating the filter.
+ BUG: certain Rodrigues and Axis-Angle combinations would cause a seg- fault due to all values being Zero. Distinguish between Errors and Warnings when automatically normalizing the Axis-Angle and Rodrigues vectors. Updating documentation to explicitly state that the utility is auto-normalizing some of the inputs.
+ BUG: DREAM3D\_UI closeEvent slot was being called twice when exiting DREAM3D from the dock menu.
+ Feature: Adding custom dock menu for Mac OS X.
+ Feature: Integrated the complete **StatsGenerator** application as a filter in DREAM.3D 

### SIMPLView Changes ###
+ Bug Fix: Fixing bug where it was possible to delete filter widgets using the delete key while the pipeline was running.
+ Added GUI for **Pause Pipeline** filter
+ Added GUI for **Execute Process** filter
+ Added GUI for **StatsGenerator** filter
+ Added GUI for **Array Calculator** filter
+ Added GUI for standard output to capture output during the **Execute Process** filter
+ Feature: Adding drag-and-drop of filters between DREAM.3D/SIMPLView windows
+ Feature: Adding a "Select All" keyboard short cut
+ Bug Fix: Fixing a bug relating to the help dialog crashing on Windows when DREAM3D closes.
+ Bug Fix: Adding high-DPI support for Windows on retina quality screens.
+ Feature: Allow reading a stack of scans from inside an EDAX HDF5 file.
+ Bug Fix: Fixing bug where changing the name of a data array or attribute matrix in the DataArrayCreationWidget or AttributeMatrixCreationWidget, respectively, would cause the name to revert back to its old name when explicitly pressing the Apply button.
+ Feature: Adding code to disable the Go button whenever preflight finishes with errors.


### SIMPLib Changes ###
+ Feature: Add infrastructure to allow Cut/Copy/Paste of filters into/out-of a pipeline view
+ Feature: Add infrastructure to allow "Undo/Redo" commands
+ Feature: Add **Execute Process** filter that allows the filter to execute any program on the computer system.
+ Feature: Add a **Pause Pipeline** filter that allows the user to pause a pipeline at a given point.
+ Feature: Added a **StatsGenerator** filter that allows the user to generate statistics for a synthetic microstructure inside a pipeline instead of using a separate application.
+ Feature: Added an **Array Calculator** filter
+ Bug Fix: Fix bug that was not allowing **Pipeline Runner** to execute properly
+ Optimize Array initialization during the **Create Data Array** filter
+ Bug Fix: Fix issue where attempting to read a single variable length string from an HDF5 file would result in random bytes copied into the buffer
+ Bug Fix: incorrect logic test in copyData() method of the DataArray class
+ Feature: Filters can now track warning conditions
+ Bug Fix: Fixing a bug that was causing the range filter parameter's values to fail during the H5FilterParameter writing process.
+ Bug Fix: Do not allocate Geometry Arrays during preflight.

### Known Issues ###

+ LINUX: DREAM.3D help opens in the user's preferred web browser instead of a DREAM.3D window.

### Developer Notes ###

+ Releases are now built against Qt 5.6.0, HDF5 1.8.16, Boost 1.60.0, Qwt 6.1.2, Eigen 3.2.8, ITK 4.9.1

