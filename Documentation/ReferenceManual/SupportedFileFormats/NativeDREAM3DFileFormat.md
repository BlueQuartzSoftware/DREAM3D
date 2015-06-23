Native DREAM.3D File Format {#nativedream3d}
===========

## HDF5 ##
DREAM3D uses the [HDF5](http://www.hdfgroup.org) as its native file format which means that **_ANY_** software package capable of reading an HDF5 file can read a DREAM3D file. HDF5 stores data in a hierarchical format with complete descriptors for the data stored in the file. HDF5 is open-source and a number of tools exist that allow a user to view and manipulate HDF5 data files. One such tool from "The HDF Group" is the free  [HDFView](http://www.hdfgroup.org/hdf-java-html/hdfview/index.html#download_hdfview),  which is a java based program that can view and edit HDF5 data files. 

Each array is stored as an individual data set in HDF5 under one of several different types of _DataContainer_ storage groups:

+ Volume DataContainer
+ SurfaceMesh DataContainer
+ SolidMesh DataContainer


Depending on the type of data created the actual data array that a user may be interested in may be stored in various subgroups in the HDF5 file. For example, with a Voxel Data Container there are six (6) types of Data: Vertex, Edge, Face, Cell, Feature and Ensemble. Arrays created for each type are stored in subgroups. For example, with Voxel based data the following HDF5 group organization is used:

+ Volume DataContainer ( _Group_ )
    + DIMENSIONS ( _Dataset_ )
    + ORIGIN ( _Dataset_ )
    + SPACING ( _Dataset_ )
    + VERTEX_DATA ( _Group_ )
    + EDGE_DATA ( _Group_ )
    + FACE_DATA ( _Group_ )
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

With a SurfaceMesh Data Container there are only five (5) types of Data: Vertex, Edge, Face, Feature and Ensemble. The missing type of Data in the SurfaceMesh is the Cell.  This is because the SurfaceMesh is a 2-D feature, and, thus, the Face is the Cell and, thus, a level can be omitted.


+ SurfaceMeshDataContainer ( _Group_ )
    + Faces ( _Dataset_ )
    + Vertices ( _Dataset_ )
    + VERTEX_DATA ( _Group_ )
        - VertexNormal ( _Dataset_ )
    + EDGE_DATA ( _Group_ )
    + FACE_DATA ( _Group_ )
        - Normals ( _Dataset_ )
        - Centroids ( _Dataset_ )
        - Curvature ( _Dataset_ )
        - IPFColor ( _Dataset_ )
    + FIELD_DATA ( _Group_ )
        - AvgCurvature ( _Dataset_ )
        - MeanWidth ( _Dataset_ )
    + ENSEMBLE_DATA ( _Group_ )
        - GBCD ( _Dataset_ )
 
In the above we show how a surface mesh is stored. For more information on the actual data structure of the surface mesh please see the [Surface Meshing Tutorial](tutorialsurfacemeshingtutorial.html).

**NOTE:** DREAM3D is perfectly capable of storing **BOTH** the voxel data and the surface mesh data in the same file if the user so desires. This allows all the data to stay together to be more easily shared among collaborators.

