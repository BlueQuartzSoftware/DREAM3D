DREAM3D Development ToDo List {#todo}
===============

This is the list of things to do. It is written with the Markdown style so it can be integrated with the other documentation pages.

## Show Stopping Bugs ##

+ StatsGen crashes if data is missing


## Version 4.2 Features Bugs to Fix ##

## Version 4.3 Features/Bugs to Fix ##

**All of this is out at GitHub.com/dream3d**

+ Rewrite the Quaternion Code to only use 4 Components and implement basic Quaternion Math functions
+ Adapt statsGenerator Color PoleFigure creator to create image based on GBCD Visualization Data
+ Pole Figure smoothing function in Visualize GBCD
+ Add option to ReadH5Ebsd to Create the DataContainer in addition to creating the .h5ebsd file.
+ Reorganize Pipeline Menu to gather together "Favorite/Prebuilt" actions versus "Pipeline Actions"
+ PipelineView Widget
  + When running a pipeline have a method that can disable all of the GUI widgets and Menus that would change the pipeline while running
    + During Execution of the pipeline dim the filters except for the one that is currently running
    + Disable modifying pipeline while a pipeline is running
    + Disable adding of filters into the pipeline while it is running.
+ StatsGenerator: ODF Tab
    - Add information for stats generator about the format of the ODF Files import function
    - Add feature in Stats Generator to read in a .ang or .ctf file for the ODF angles.
    - Add feature when reading ODF Euler Angle files to convert data from Radians to degrees.
    - Modify ODF/PF generation in stats generator to better combine random baseline with user inputs (also degree/radian checkbox)
+ Ghost data is generated in the ODF in Stats Generator for certain orientations (90,35,45 1000, 3)
+ Write out Lattice Constants from EBSD data to XDMF data
+ Modify filter to dump Images by adding ability to dump images for XY, YZ, XZ planes to a folder.
+ Integration of Image Processing Library into DREAM3D
+ Image Data: Need way of reducing RGBA to RGB array so export to XDMF can work properly for image data
+ Add Filter to set Phase/Ensemble data instead of reading it from a file (Custom Gui for this one).
+ Create filter to generate Abaqus input files, Albert Cerrone or Curt Bronkhorst to help with this


### Critical to Fix Before Bad Things Happen ###
**All of this is out at GitHub.com/dream3d**
+ OrientationOps classes need a redesign so that we don't crash when we hit an unknown crystal structure.
+ All the shape ops classes need to be fully reentrant for parallel applications

### Joey K. Summer List ###
**All of this is out at GitHub.com/dream3d**
+ Copy Pipeline from .dream3d into new .dream3d during pipeline execution
+ Import Pipeline from a .dream3d file
    - Add ability to append to pipeline folder in .dream3d file when starting a new pipeline with the read dream3d filter
+ Create Dialog that contains prebuilt stats files for StatsGenerator users to select from
+ When loading a Pipeline file detect any paths that are the wrong platform (C: on Unix) and offer to pick a new file
+ Create a "ExamplePlugin" that has one of every FilterParameter Type
    - Create a new Plugin called "ExamplePlugin"
    - Jackson has most of the code done.
    - break up codes into a few filters
    - The actual source code you will need to implement the _setupFilterParameters()_ function. You can copy/paste from other filters to build up the list of every parameter type.
    - This will make that when **FilterWidgetCodeGen** is run we are truly testing every type of parameter



### Documentation ###
**All of this is out at GitHub.com/dream3d**
+ Update Documentation for Visualize GBCD
+ Tutorial::Small IN100 for Docs/Website
+ Tutorial::Synthetic Generation for Docs/Website
+ Tutorial::Visualization Techniques for Docs/Website
+ Tutorial::Creating Ensemble Statistics for Docs/Website
+ Tutorial::Importing Orientation Data for Docs/Website
+ FileFormats::Xdmf files for Docs/Website
+ Update/Create a user manual for the stats generator - last parts need pasting
+ Update documentation file structure now that users can specify array names
+ Update StatsGen documentation regarding the file format for loading ODF Angles.


### Workshop Feedback
**All of this is out at GitHub.com/dream3d**
+ SurfaceMesh Face IPF Color display something about normals/orientation (Nathalie Gey's Question at workshop during surface meshing session)
+ Insert a "favorite" in between other filters, instead of only at the end of a pipeline
+ Write a .med file for the Salome FEM package which is just an HDF5 file.

### Developer Centric ###
**All of this is out at GitHub.com/dream3d**
+ Add documentation on how to add new filter directly into DREAM3D
+ Add documentation on how to add new filter group directly into DREAM3D


### Statistics/Analysis/Other Algorithms ###
**All of this is out at GitHub.com/dream3d**
+ Add filter to calculate histograms of user selected arrays (cell or field)
+ Add filter to insert subgrains
+ Fix the deformation statistics filter to adapt to new structure of transmission metrics
+ Add Filter to initialize the Ensemble Map based on a user defined array such as Phase Ids.
+ Integrate Lisa Chan's Twin Insertion Code - Have Code From Joe Tucker
+ Change Field to Region in all codes
+ Add/Modify filter to note if grain is a twin, parent or untwinned
+ Add ability to define twins of interest on merge twin filter
+ Add ability to flag regions as 'defects' or a new phase in the fill bad data filter
+ Look into incorporating genetic algorithms in the packing algorithms of the synthetic builder
+ Include a filter to calculate 2-pt statistics
+ Look into using 2-pt statistics to advance the packing algorithms in the synthetic builder
+ Add a filter to calculate Kearn's number
+ Look into matching the Kearn's number when matching cystallography in the synthetic builder
+ Add a filter to calculate 'clustering' statistics on 'marked' features
+ Add/Finish filter to calculate slice-slice misorientation (in terms of axis-angle in sample ref frame)
+ Add filter to calculate microtexture function or equivalent
+ Look into creating a reverse Saltykov/Sahagian filter to predict 3D grain size distribution from 2D data
+ Add filter to merge twins using surface mesh (5 parameter)
+ Add virtual sectioning filter
+ Add/Finish filter to 'standardize' euler angles within a grain
+ Look into creating a filter to color orientations using C. Shuh's color scale
+ Add mesh quantification filter (code from Tony)
+ Add 3D linear intercept calculation filter (C. Hartley filter)
+ Add filter to perform Tony's hot spot analysis
+ Add filter to combine far field data with near field data from HEDM
+ Add filter to determine spherical correlation length from each grain centroid
+ Add mean filter in orientation space
+ Add mean filter on scalar array
+ Add median filter on scalar array
+ Fix the mPrime, F1, F1spt, F7 functions in Hex- and Ortho- Ops


### Still to Do ###
**All of this is out at GitHub.com/dream3d**
+ "Tools" menu launches new instances of DREAM3D/StatsGenerator on Windows/Linux (Bummer)
+ Comprehensive search of the help system (Possibly QAssistant based?)
+ Put an option on the DREAM3D file writer to write the voxel data container as a solid mesh instead of the XDMF rectilinear grid.
+ ImageJ exporter codes
+ Add option to each filter to pause the pipeline. See DevelopmentNotes.md
+ Fix parent IDs not being initialized in merge twins, beta grains, etc.
+ Disregard option in Segment Grains (more local merge twin filter)
+ more general grouping filter. Be able to specify orientations to group by and other stuff.
+ Option to use local orientation to use the Surface Mesh Face IPF coloring rather than use the average for the grain
+ Add "Tool" example program that shows writing a command line pipeline program
+ Add checks in MinSize and MinNeighbor filters to exit gracefully if minimum is larger than largest grain/num neighbors
+ Add in Xdmf wrapper for H5Ebsd so they can be easily viewed in ParaView
+ Write program to read in Pipeline Files on the command line
+ Writing NonConformal Surface Mesh does not take into account values whose sign is dependent on the winding of the triangle.
+ Add in the HyperSlabArray class to use for some of the SurfaceMeshing Triangle/Node data
+ Sometimes dragging a filter from the filter list to the PipelineView area will cause a crash.
+ Filter that can execute any program on the users computer
+ Look into creating multiple maps at each level (i.e. parent and grain filed maps)
+ Create filter to generate surface mesh from volume mesh
+ Design protocol to learn user's 'ontology'
+ Allow the user to specify the phase for minimum neighbors filter
+ Look at adding a mirror option to the rotate sample reference frame filter
+ Add the option for the user to set the name of a copied array (from field to cell)
+ Work through all filters and change single for loops over totalPoints to triple loops over x-,y- and zPoints if the coordinates need to be calculated


###  Synthetic Building Filters ###
**All of this is out at GitHub.com/dream3d**
+ Add ability to read list of ellipsoids into packing algorithms
+ Look into bias near outside of box during synthetic building
+ Fix synthetic builder to note if volume is 2D and actually pack slices


###  IO Filters ###
**All of this is out at GitHub.com/dream3d**
+ Add output directory to the HexToSquare Grid convertor filter
+ Provide better feedback on 3D .ctf files (No way to do this without major changes to codes)
+ Add filter to write out slices as images
+ Add ability to store EBSPs and EDS spectra in h5ebsd file and data container
+ Add filter to write out 2D slices as .ang, .ctf or .mic files
+ Add filter to dump ensemble stats/info
+ Add filter to read .edf data
+ Read in more meta-data about each phase: Space Group, Phase Name, Material Name


### Feature Request ###
**All of this is out at GitHub.com/dream3d**
+ A filter that finds all the twins in the microstructure and assigns them to another phase (phase 3), so that they can be visualized differently to the surrounding grains (phase 2) and grains that are untwined (phase 1).
+ Add splash Screen similar to ParaView where the various entities that have donated code or Funding are shown with their logos.

###  Surface Meshing ###
**All of this is out at GitHub.com/dream3d**
+ Add mesh decimation filter (surface or volume)
+ Add Marching TetraHedra surface Meshing Filter


###  MFE Smoothing ###
**All of this is out at GitHub.com/dream3d**
+ add ability to stop the smoothing
+ bug in edges file is stopping the use in the smoothing filter

###  Bruker EBSD System Support ###

+ BASE64 decoder
+ XML Parser for the SEM Image file
+ Create XML Parsing code for each file from the Bruker File Archive
+ Include Expat as part of the DREAM3D Compile?


## Items Completed ##
**All of this is out at GitHub.com/dream3d**

### Version 4.2 Fixed ###


##  No Longer Wanted ##
+ Add structured grid writer

