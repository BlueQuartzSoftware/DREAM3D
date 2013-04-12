Native DREAM3D File Format {#nativedream3d}
===========

## HDF5 ##
DREAM3D uses the [HDF5](http://www.hdfgroup.org) as its native file format. HDF5 stores data in a hierarchical format with complete descriptors for the data stored in the file. HDF5 is open-source and a number of tools exist that allow a user to view and manipulate HDF5 data files. Once such tool is the free  [HDFView](http://www.hdfgroup.org/hdf-java-html/hdfview/index.html#download_hdfview) from "The HDF Group" which is a java based program that can view and edit HDF5 data files. 

Each array is stored as an individual data set in HDF5 under one of several different types of _DataContainer_ storage groups.

+ Voxel DataContainer
+ SurfaceMesh DataContainer
+ SolidMesh DataContainer


Depending on the type of data created the actual data array that a user may be interested in may be stored in various subgroups in the HDF5 file. For example with a Voxel Data Container there are three types of Data: Cell, Field and Ensemble. Arrays created for each type are stored in subgroups. For example with Voxel based data the following HDF5 group organization is used:

+ VoxelDataContainer ( _Group_ )
    + DIMENSIONS ( _Dataset_ )
    + ORIGIN ( _Dataset_ )
    + SPACING ( _Dataset_ )
    + CELL_DATA ( _Group_ )
        - GrainIds ( _Dataset_ )
        - EulerAngles ( _Dataset_ )
        - IPFColors ( _Dataset_ )
    + FIELD_DATA ( _Group_ )
        - Phases ( _Dataset_ )
        - AvgEulerAngle ( _Dataset_ )
    + ENSEMBLE_DATA ( _Group_ )
        - CrystalStructures ( _Dataset_ )
        - MaterialName  ( _Dataset_ )

In the above example we have shown a number of data arrays for each group type. The user should note that the data sets shown in the above example are hypothetical and may or may not appear in data sets that the user generates.

+ SurfaceMeshDataContainer ( _Group_ )
    + Faces ( _Dataset_ )
    + Vertices ( _Dataset_ )
    + CELL_DATA ( _Group_ )
        - Normals ( _Dataset_ )
        - Centroids ( _Dataset_ )
        - Curvature ( _Dataset_ )
        - IPFColor ( _Dataset_ )
    + POINT_DATA ( _Group_ )
        - VertexNormal ( _Dataset_ )
 
In the above we show how a surface mesh is stored. For more information on the actual data structure of the surface mesh please see the [Surface Meshing Tutorial](tutorialsurfacemeshingtutorial.html).

**NOTE:** DREAM3D is perfectly capable of storing **BOTH** the voxel data and the surface mesh data in the same file if the user so desires. This allows all the data to stay together and more easily shared among collaborators.

