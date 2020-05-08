# Version 6.6 Release Notes #

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/bluequartzsoftware/DREAM3D/issues).

## Getting Help ##

The developers of DREAM.3D maintain a pair of Google Groups for discussions on the use of DREAM.3D and the development of filters or plugins for DREAM.3D. If you have a question those are the best places to ask and get help from the community.

[DREAM.3D Users List](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[DREAM.3D Developers List](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.6.xxx ##

### 6.6.xxx ###

+ Fix crash inside of Insert Atoms.	MicroClue <whsiao1995@hotmail.com>
+ Added LinkedPathCreationFilterParameter
+ Updated IGeometry API
+ EbsdMontage filter and infrastructure additions (NRL funded)
+ Fixes to SPParksDumpReader
+ Fixes to AbaqusHexahedronWriter
+ Fix crash in Initialize Synthetic Volume
+ All orientation operations are now done with double precision where possible
+ Fix potential crash when using an RDF in Insert Precipitates (#922)
+ Rotate Sample Reference Frame now can use a rotation matrix
+ Insert Precipitate Phases:Fix bug where using a mask would not differentiate between phases.
+ Can retrieve the space group number from each Laue Class
+ Added backward compatibility for Qt 5.9.9 through Qt 5.14.x
+ Added python bindings that are optionally generated during the build process
+ remove deprecated uses of TBB headers
+ H5Support is now a separate library (http://www.github.com/bluequartzsoftware/H5Support)
+ EbsdLib is now a separate library (http://www.github.com/bluequartzsoftware/EbsdLib)
+ Some documentation for StatsGenerator was updated
+ Improved usability for RegularGridSampleSurfaceMesh
+ Allow selection of any input file when bulk loading angles in the StatsGenerator filter
+ Initialize Synthetic Volume can not copy and existing Image Geometry which helps usability
+ Create Data Array can now init each component of a multi-component array.

