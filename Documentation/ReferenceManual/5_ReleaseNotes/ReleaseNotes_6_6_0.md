# Version 6.6 Release Notes #

These release notes summarize the major changes for each officially released version of DREAM.3D. The official bug report is located at the [DREAM.3D GitHub issues page](https://github.com/bluequartzsoftware/DREAM3D/issues).

## Getting Help ##

The developers of DREAM.3D maintain a pair of Google Groups for discussions on the use of DREAM.3D and the development of filters or plugins for DREAM.3D. If you have a question those are the best places to ask and get help from the community.

[DREAM.3D Users List](https://groups.google.com/forum/?/dream3d-users#!forum/dream3d-users)

[DREAM.3D Developers List](https://groups.google.com/forum/?hl=en#!forum/dream3d-developers)

## Version 6.6.xxx ##

### 6.6.xxx ###

#### SIMPL Changes ###

Place newest Commits at the top of this list

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
+ Initialize Synthetic Volume can not copy an existing Image Geometry which helps usability
+ Create Data Array can now init each component of a multi-component array.
+ Add PartitionGeometry Filter
+ StringDataArray: isAllocated would always return true. 
+ Fix sanity check for feature Ids greater than num tuples in a feature attribute matrix	
+ InitializeDataImpl: Remove unneeded include.	
+ ImageGeom::getIndex - Return an empty value if x/y/z are out of range.	
+ Allow Drag and Drop to LinkedDataContainerSelection Parameter	
+ Add missing CSV file for use with Create RectGrid Geometry example pipeline.	
+ CopyFeatureArrayToElementArray: Relax restrictions on number of features.	
+ Allow AttributeMatrix and DataArraySelection Requirements to take a vector of types	
+ Fix MacOS 10.13 compile errors.
+ Updating licenses.	
+ Update ImageGeom getIndex method with correct algorithm.	
+ StringDatArray: Only allocate the internal vector is asked to do so.
+ Adding PadImageGeometry filter with unit test.	
+ Fix crash when trying to generate a Help URL from a null factory pointer	
+ ColorToGrayScale: fix casting issue in the luminosity algorithm	
+ MultiThresholdObjects/MTO2 can now output arrays as any type.	
+ Remove InActiveObjects: Actually remove the NeighborList Arrays.	
+ Parallelize InitializeData filter.	
+ Update ConvertData to handle boolean arrays correctly.
+ Update Initialize Data to work with boolean arrays.
+ Update CreateDataArray docs to include Initialization Types.	
+ Updating CreateDataArray to include Indices initialization option.
+ Add DateTime stamps to the start of each filter execution.
+ Add Accept button to FileListInfoWidget
+ Fix issue where NeighborList could not be selected for a path.	
+ RotateSampleRefFrame: Fix long preflight issues. Parallel Rotations.	
+ Adding new StringUtilities and StringLiteral files.
+ Updated filter parameters to utilize a group index array instead of a single group index.

#### DREAM3D Changes (Since v6.6.0 Tag) ####

Place newest Commits at the top of this list

+ Fixed crash in FindFeatureFaceCurvatureFilter
+ Added option to save the Weingarten Matrix from FindFeatureFaceCurvatureFilter
+ FeatureInfoReader: Allowed AM Types were never set for the Element Attribute Matrix
+ Fix sanity check for feature Ids greater than num tuples in a feature attribute matrix
+ ResampleImageFilter: Remove strict dependency on TBB.
+ PartitionGeometry: Update Pybind11 properties.
+ PartitionGeometry: Show OOB Part. ID for both Advanced and Bounding Box.
+ PartitionGeometry: Update human label for the AttributeMatrix selection
+ ReadSTLFile: Create vertex attribute matrix during filter
+ Tweaking hexahedral geometry tests, removing debug code, updating docs.
+ CMake: Remove QtGui from unit test where appropriate.
+ ReadSTLFile: Add creation of Vertex AttributeMatrix.
+ Adding tests for Basic and BoundingBox modes.
+ Remove unneeded include statement.
+ Remove GUI dependency in Reconstruction plugin testing.
+ ReadSTL: Add an option to scale the resulting vertices from the STL File.
+ Fixing failing PartitionGeometry unit test.
+ Updating some labels, using only two points for BoundingBox mode.
+ Update partitioning geometry string to display messages when geometry data is not available.
+ Adding Simple and Bounding Box modes to PartitionGeometry filter.
+ Fix compile errors.
+ Add PartitionGeometry unit test.
+ Updating licenses.
+ Write out partitionIds array as a point array for node based geometries. Allow users to write out the partitioning scheme, if desired. Update the preflight updated values to have more precise names.
+ Tweak filter parameters, add preflight updated values.
+ Add new PartitionGeometry filter.  Still needs tweaking and a unit test.
+ Fix CropImageGeometry dataCheck when saving as new DataContainer
+ MinSize: Use Number of Elements consistently.
+ Update to latest ParallelDataAlgorithm APIs
+ FindKernelAvgMisorientations: Parallelize and allow cancel
+ Update OrientationUtility to calculate Stereographic coords.
+ QEbsdReferenceFrameDialog: Remove help button. Crashed D3D anyways.
+ Minor updates to clean up code
+ Update Pybind properties.
+ Expose Randomize Feature Ids check box.
+ Fix non cubic-high (m3m) check in MergeTwins filter
+ NeighborOrientationCorrelation: Fix typo in variable name that was causing output errors.
+ Correcting a mistake in insert precipitate phases where the origin wasn't being subtracted properly. (#997)
+ Fix issue where MDF table would not accept changes entered by the user.
+ Update ReadMe.md file to contain more explicit python install directions
+ Fixes to FindSurfaceFeatures
+ FindBoundingBoxFeatures: Fix bug where other 2D planes are not considered for XY coords
+ Add sanity check and error message to Calculate Ensemble Statistics.
+ Adding native visualize for Qt types. This is ONLY for Clion based projects. Visual Studio and QtCreator have what they need
+ Update H5Support to bring in the H5Support namespace
+ Update ReadMe with dream3d-conda information
+ Add sphinx generated python API documentation.
+ Create option to build the DREAM3D Development tools. On by default.
+ Remove unnecessary HDF5 detection. Add in proper link libaries
+ use FQDN when copying files on windows
+ EMMPMLib: Fix install rules for conda packaging
+ Update to latest API change for the build date as version.
+ Ensure the GUI plugins are also installed correctly.
+ Refactor PluginProperties() CMake Macro.
+ Fixes to allow building conda package on macOS
+ Updated parameter widgets to use new exception safe getter/setter
+ OrientationAnalysis plugin was still linking against a QtGui library.
+ Adjust conda-build rule for copying setup.py
+ Add the python development docs to the docs folder.
+ QuickSurfaceMesh: Add option to skip voxel fixes. (#980)
+ FindFeatureCentroids: Use Kahan Summation instead of naive summation
+ Validate all input data arrays have the same number of tuples
+ Mark the Conda dir as deprecated. Should the official repo instead to create dream3d-conda packages.
+ Fixed python DataArray constructors
+ Applied clang-format
+ Increment the dream3d-conda version to 1.1.0
+ Added python stub generation to anaconda package
+ Changed setup.py python package version to 1.0.0 to match meta.yaml
+ Fixed statstoolbox python imports
+ DOC: Add unix operating system instructions for developing python codes.
+ Rename StatisticsPlugins to StatsToolbox. (#979)
+ Missing override keywords
+ OCD got the best of me. Had to fix the spelling mistake in the license text
+ DOC: Add details to build a python dev environment from scratch.
+ Added documentation for writing Python filters and python codes
+ Fix use of virtual in the Plugin Header files
+ Removing unnecessary EbsdLib includes and link libraries
+ Fix crashing issue when incorrect/mismatched DataArray is selected.
+ Add warning to MergeTwins if non Cubic-High (m3m) is used
+ Fixed crashing bug due to nullptr return.
+ Remove stale code that has a creative-commons non-commercial license.
+ Remove use of QwtArray and replace with QVector
+ InsertPrecipitatePhases: Check for cancel more often.
+ IdentifySample can now use a bool or uint8 array as the mask.
+ More robust ASCII or Binary STL file detection.
+ Changed menu json name to match conda package name
+ Replaced manual plugin list with jinja template
+ Fixed some conda macOS build issues
+ Added start menu item when installing DREAM3D conda package
+ DOC: Update the Anaconda build documentation
+ Changed writeJson to be const
+ Added some instruction on using Python filters with FilterPipeline
+ Update Conda Package version of DREAM.3D
+ Remove obsolete files related conda packaging and python wrapping
+ Embedded Python (#976)
+ Correct calculation issues in FindShapes by using code from Eigen (#974)
+ Double quote some cmake directory paths.
+ Fix issue where the z spacing would always be zero.
+ Add feedback to IdentifySample filter
+ Suppress Qwt Deprecation warnings. Cleanup other compiler warnings
+ Fix compiler warnings and small code cleanups
+ Replaced Qt keywords
+ Removed stray comment
+ Fix RenumberFeatures bug in CropImageGeometry filter
+ Fixed typo in Summary.cmake
+ Changed some FilterParameters to use std::vector
+ Remove option to build against QtWebEngine
+ Formatted Summary.cmake
+ Replaced filter compatibility checks with ones from SIMPL
+ Removed extra ChangeResolution.cpp
+ Removed Python bindings for EMMPMFilterParameter
+ Topic/filter member variable init (#969)
+ Changed filter parameter bindings to derive from FilterParameter
+ Throw exceptions if filter or filter parameter are not compatible.
+ Added CMakeUserPresets.json to the gitignore
+ Replace use of qproperties with FilterParameter callbacks instead (#968)
+ Change AttributeMatrix/IGeometry Types Enumeration to std::vector from QVector (#967)
+ Rewrite ChangeResolution filter to fix bugs and crashes (#966)
+ Changed member variable to const reference in ReadStlFile
+ Fix compiler warnings about Qt 5.15 deprecation through QtBackwardsCompatibility.h
+ Add a Summary.cmake file to print out the configuration summary
+ Fix include paths that were updated in Qt 5.15
+ Fix issue where loading a synthetic pipeline with a Matrix Phase was incorrect
+ Create GenerateVectorColors unit test
+ Fix compile errors due to copy/paste from the 6.5 branch
+ Fix issue where NaN could be produced in AddOrientationNoise.
+ Theme names and file paths are independent of each other.
+ Detect ASCI STL file and error out.
+ ReadStlFile: Check if the filter was canceled during the read loop.
+ Added clang-format check GitHub workflow
+ Applied clang-format
+ Fixed usage of DEFAULT_BLOCKSIZE
+ Fix bug in VTKRectilinear Writer.
+ Generate FZ Quaternions was not copying the final Quat back to the output array
+ Disabled ResampleRectGridToImageGeomTest as it's unimplemented
+ Fixed warning about unused variable in catch block
+ Fixed a performance regression in CropImageGeometry
+ Changed SIMPLib::Constants to inline constexpr template variables
+ Do NOT install the H5Support headers during packaging
+ Fix issue where voxels with the same quaternion would not group together.
+ Changed classes in GenerateFeatureIds.h to functions
+ Moved EnsembleInfoReaderTest to OrientationAnalysis
+ Moved ExportDataTest to SIMPL
+ Enable all IO Unit Tests
+ SPParksDumpReader Improvements: Can now read values in floating point styles
+ Revert part of last commit to FindAvgOrientations.h
+ Add a unit test for FindAvgOrientations Filter
+ Update all Quaternion related codes to new API.
+ Fix bug where not all data sets would be read. Also fix nullptr crashes.
+ Compute Volume Fractions for all phases.
+ Update documentation to reflect output units of the data
+ Fixed alloc/dealloc mismatch in FindTriangleGeomShapes
+ Fixed alloc/dealloc mismatch in FindShapes
+ Fixed double close of HDF5 objects
+ Changed generated constants for test files to be inline
+ Changed to pass HDF5 ids by value to scoped sentinels
+ Fixed out of bounds memory access
+ Fixed inconsistent overrides
+ Fixed mkdocs command for SDK changes
+ Set EbsdLib_INSTALL_LIB property when needed
+ Missing unit test for ResampleRectGridToImageGeom
+ RectGrid Resample to ImageGeom (#949)
+ Removing unnecessary comments
+ Feature/python assert (#958)
+ Correct comment to remove dead URL.
+ Update CalculateArrayHistogram to display the correct data container name.
+ Ebsd API Updates (EbsdLib removed dependency on Qt5) (#953)
+ Changes to allow compiling without TBB support
+ Update MDF Table to display values according to the current locale.
+ Allow user to select cell attribute matrix in AlignSectionsList. (#957)
+ Fix parsing bugs of MDF Table Data. Fully read input MDF file.
+ Quaternion API Updates (#951)
+ Quiet deprecation warnings using the backward compatibility macros
+ SampleSurfaceMesh: General code cleanup in combination with lower level fixes in SIMPL
+ Fix bug that was introduced in 1ab713bcdc39c where variable were never set.
+ Updated to C++ 17
+ Update Clang Tidy to report unused return values
+ Use 'Complete' version for plugins which includes the Git Hash
+ Fixed typo in Python test name
+ Example Python scripts to batch process files or drive a pipeline fro… (#948)
+ Fix clang-format typo: Tabs should be 2 spaces
+ Clang format sorts headers more logically.
+ Fixed signal connection in PrimaryPhaseWidget
+ Implemented missing getUuid() for some filters
+ Fix errors when generating MDF Plot data in StatsGenerator (#947)
+ Fixed MSVC warning about old C style struct typedef definitions
+ Merge Documentation Updates from SIMPL PR #402
+ properly react to return value in fscanf()
+ Allow additional Laue classes when creating synthetic microstructures.
+ Fix typo in example pipelines that involve create data array
+ Formatted all source files with clang-format
+ Use std::vector<double> instead of QVector<double>
+ H5Ebsd: make all error codes unique.
+ Start the release notes for 6.6 release
+ Fix for failing synthetic tests due to API additions in Initialize Syn Volume
+ Remove extra space and fix some include groupings
+ Remove extra include for <memory> since it is already in the header file
+ Allow Initialize Synthetic Volume to copy ImageGeometry from another Data Container (#943)
+ Allow user to select .txt, .csv or any file when bulk loading angles
+ DOC: Update repository and folder layout when downloading sources
+ Check return values when reading from files and act on those values (#941)
+ Update the StatsGen documentation to match the latest UI (#940)
+ Code cleanup. Set default length units to micrometer
+ Use an IntVec3 to gather the dimensions. Add box size as feedback
+ Fixing typo in the documentation
+ Added extra status messages about CMake Python variables
+ Removes support for 'ExternalProjects' folder inside of DREAM3D
+ On macOS systems, do not attempt an install of EBSDLib
+ Remove tbb/atomic.h as it is deprecated in the latest TBB library release
+ Set the EbsdLib properties to install header and library files.
+ Add property to set the install location for EbsdLib
+ New Python bindings generator (#939)
+ Remove unit test that does not do anything for ImportEbsdMontage
+ Fix runtime error due to code that got deleted.
+ Remove uses of tbb::task_scheduler_init from all codes
+ Additional paths and versions for dependent libraries are printed at end of CMake run
+ Properly check for a valid Python and Mkdocs executables on macOS systems
+ Minimum CMake version is set to 3.14
+ # Merge H5Support and EbsdLib stand alone libraries
+ API updates in OrientationLib (#937)
+ Montage Data Structure (#936)
+ # Merge Python wrapping macro changes
+ Fixes issue when reading a version 7 .ang file. (#935)
+ Backward compatibility for Qt 5.9 to 5.14 (#933)
+ Changes to data structure and AbstractFilter API (#932)
+ Fixed ClassName and getNameOfClass functions
+ Fix crash when badly formatted SPPARKS Dump files are used.
+ Fix issue where Feature Info Reader would not read all data in the file
+ Update .gitignore
+ Fixing space group number to Laue Ops instance edge-case bug. (#929)
+ Space group number to laue ops instance. (#928)
+ Updating TestFileLocations.h templates to use correct template file path.
+ Add some more Git Hash to the end of the configuration.
+ Update test to API changes in JsonFilterParameterWriter
+ Remove AppVeyor files.
+ DOCS: Update the ReadMe with more explicit links to download the binaries
+ Fix Python Binding runtime issue for missing symbol (#926)
+ Add missing include directive
+ Removed use of "C" style time functions.
+ DOC: Improving the Pack Primary Phases documentation
+ Remove CI related files.
+ Update the programming style guide with updated rules for C++11
+ Fix bug where using a mask would not differentiate between phases.
+ Feature/move rotate filter (#924)
+ Update the macOS build instructions.
+ Fix alias to use a const where appropriate
+ Update all FileListInfo uses to the proper subclass.
+ MKDOCS: Use specified python executable to run mkdocs (#923)
+ Fix potential crash when using an RDF in Insert Precipitates
+ Remove backslash from comment. Oops.
+ Update RemoveSIMPLMacros generated source codes to be const correct.
+ Update Sandbox Tool to generate proper codes for filters.
+ Update codes to remove use of macros, fix python errors, fix const issues
+ Changed ModifiedLambertProjectionArray functions to be const (#919)
+ Fix compile error that got left out on last merge
+ Convert all orientation operations to double precision where possible (#916)
+ Fix ITK5 Symbol Loading on MacOS (#918)
+ Forward port fixed issues from the 6.5.128 branch
+ Fixed a crash due to an unallocated DataArray in filter (#915)
+ Feature/912 import ebsd montage (#913)
+ Updates to example python codes to allow them to properly execute (#911)
+ Last Commit went to wrong branch causing Compiler error.
+ RUN: Fix errors in Abaqus Exporter and SPParks Dump Reader
+ DOCS: Allow mkdocs to properly generate the documentation
+ Fix compile error introduced during a merge conflict resolution
+ Fix issue in the configuration file for the mkdocs builder
+ Tweaks to Sandbox Tool
+ Print out the git hash and commit date for each plugin
+ Update DataArray API constructors to decrease the chance of ambigious overloads
+ Remove combination of std::for_each() and lambda with a rand based loop.
+ Updated codes to latest IDataArray/DataArray API Changes
+ Use a DLL export macro for EbsdMontageListInfo
+ Merge remote-tracking branch 'upstream/develop' into develop
+ Topic/add ipfcolors option to ebsdmontage (#908)
+ SandboxTool to replace SIMPL Macros with proper code.
+ Remove unused Qt5 include directives (#907)
+ Updated GenerateIPFColors with new ParallelDataAlgorithm (#906)
+ Fix compiler warnings for signed/unsigned comparisons.
+ Use generic terms in the CTest template files.
+ Update the ASTM638 Synthetic to execute faster
+ Properly detect the build type being used for the prebuilt tests
+ Allow debug versions of the executables to run on macOS/Linux
+ Fix runtime issue in Laplacian that would not allow selection of data array path
+ Sanity Check the existence of the input file
+ Create Separate CTest tests for each prebuilt and example pipeline (#903)
+ Update docs with preferred markdown heading 1 style
+ Fix the ASTMD638 Example pipeline to produce correct output.
+ Fix flawed logic from earlier commits.
+ Improvements to the EbsdMontage Import Filter
+ Ensure that file extensions are checked properly.
+ Update mesh based Geometry calls to use new API in SIMPL
+ Fix compiler warning about anonymous namespace
+ Fix compiler Warnings about mismatched signed/unsigned comparisons
+ Python: Update plugins to use correct OS specfic file extension.
+ Use the proper file extension for bash or command prompt depending on OS.
+ More verbose output when cloning the repos
+ Initial Azure Pipeline Configuration
+ Fix failing Python unit test. Adjusting an error code in ReadH5Ebsd
+ IGeometryGrid API Update, Pythong Fixes, MXA Removal
+ Introduced commit message template (#868)
+ Feature/linked path parameter (#898)
+ Fix crash inside of Insert Atoms.
+ Fix compile errors due to incorrect PYBIND macro argument.
+ Fix compiler errors due to missing DLL Export Macro.
+ Add filter to convert between quaternion representations.
+ Fix compiler warnings about unused variables.
+ Update the open source iconography based off the latest Branded DREAM.3D Styles
+ Fix compile error that was not caught in previous commits.
+ Matching changes to allow Reveal in Finder/Explorer to work correctly
+ Add options to ReadCtfFile to convert Eulers and Hex X-Axis (#897)
+ Feature/MessagesRedesign (#894)
+ Correct some Eigen output. Bring in the 6.5.121 release notes

