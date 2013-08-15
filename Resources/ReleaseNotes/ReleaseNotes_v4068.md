### Fixed for Version 4.0.638 ###
+ Fix html Help for "Standardize Euler Angles"
+ Fix html Help for "Erode Dilate Coordination Number"
+ Fix html for FindSlicetoSliceRotations.html
+ Fix html for FindSlipTransmissionMetrics.html
+ Fix html for AddBadData.html
+ Fix html for AddNoise.html
+ fix HTML for JumbleOrientations.html
+ Missing Documentation File for GenericFilters/LinkFieldMapToCellArray
+ Missing Documentation File for GenericFilters/RenameCellArray
+ Missing Documentation File for GenericFilters/RenameFieldArray
+ Missing Documentation File for StatisticsFilters/GenerateEnsembleStatistics
+ Missing Documentation File for StatisticsFilters/FindMisorientations
+ RawBinaryReader does not honor the Origin value
+ Design/Implement better system to write up all the documentation but be able to convert to PDF/HTML/etc
+ Tutorial on Surface Meshing for Docs/Website
+ Add filter to generate IPF colors for triangles in surface mesh
+ Fix reference frame button on import filter to not reset to default
+ Add node type calculation during quick mesh
+ Change the Slip Transmission Metrics filter to store the metrics as Neighborlist values to save space and time for calculation.
+ Fix data check in numerous filters that are omitting the crystal structures check and performing it in the execute
+ Fix rename cell and field array filters to update the data container during the preflight
+ Fix the link field map filter to return an error during preflight if array is not correct type
+ Add filter to rename array from cell map
+ Add filter to rename array from field map
+ Add Filter to initialize the Field Map based on a user defined cell array such as Grain Ids.
+ Move FindNeighbors filter to the statistics group
+ Add some optimizations into the PackPrimaryGrains by creating constants and removing triple nested vectors.
+ Add ability to write out goal statistics/attributes for grains during packing synthetic volumes
+ Look into deleting field arrays after packing in synthetic structures
+ Fix distributionOps functions to not fart when there is only 1 grain in a bin.
+ Output file similar to TSL OIM Analysis software. See example from Brad Fromm (INLFileWriter filter)
+ Fix TotalSurfaceAreas in MatchCrystallography
+ Fix phasetype sizecheck in GenerateEnsembleStatistics.
+ Check error causing crash in the find mdf filter
+ Hookup setting of PhaseTypes from GUI in GenerateEnsembleStatistics filter
+ Finish implementing writing of GenerateEnsembleStatistics filter'S GUI values to pref file
+ Update the H5EbsdVolumeReader to only read those arrays asked for by the user or by the QualityMetricFilters
+ Finish updating ReadH5Ebsd filter Gui for HKL and HEDM data sets
+ Add advanced button on import filter to allow any recommended transformation
+ Add in a "Search Box" to top of Filter list to filter out the list based on keywords
+ Combine the threshold_points and DetermineGoodVoxels into a single filter
+ Add ability to "add filter" to plugin maker
+ Work through all IO filters to have extensions in the save and open popup box
+ Fix issue with writing euler angles to binary vtk file
+ Fix stats generator to display new phase when new phase added
+ Add option to choose IPF coloring direction
+ Add ability to sort through filters by name
+ Add helper code to populate edge table in surface meshes
+ Update import filter to have default manufacturer transformation options
+ Add filter abilities to read .tif or other image types into array
+ Add surface mesh data container writer
+ Add framework to pass contents of data continer to filters for display on GUIs in real-time
+ Add single threshold filter for cells
+ Add multi parameter threshold filter for cells
+ Add single threshold filter for fields
+ Add multi parameter threshold filter for fields
+ Add ability to remove arrays out of maps
+ Fix the scaling of the axes in the stats generator when values are changed
+ Add import and reading capability for .mic files from HEDM data
+ Modify read dream3d filter to allow user to specify arrays to read
+ Add grain reference misorientation filter for c-axis
+ Add orientation jumble filter
+ Add filter to determine average c-axis orientation for fields
+ Add clear data filter to delete data from user defined subsets
+ Add filter to reconstruct prior beta grain during initial segmentation
+ Fix crashing bug in insert precipitates filter
+ Confirm functionality of orientation ops functions (FZ specifically)
+ Add quick mesh filter
+ Add option to align feature centroid filter to use reference slice
+ Add coordination number based erode/dilate filter
+ Add filter to create "noise" in orientations in synthetic structures
+ Add filter to insert "bad data" in synthetic structures (based on location)
+ Add filter to "identify sample" (operates on good voxels)
+ Rework deformation statistics filter
+ Add filter to calculate boundary transmission paraters
+ Create a raw binary reader
+ Move reference frame transformations to readh5ebsd filter by calling out to rotation filters
+ Generalize possible rotations in the reference rotation filters
+ Add ability to store parent id at the field level
+ Add filter to create a cell array from a field array
+ Add storage of misorientations as list at field level for each grain in find mdf filter
+ Add writing of neighborlist field level arrays to csv file
+ Add filter or option to write out misorientation values
+ Add option to write neighborlist data to csv field data file
+ Add subgroups for further organizing filters
+ Add filter to flatten color image to grayscale
+ Add filter to segment features by scalar tolerance value
+ Modify all stats filters to check for 2D in all directions

