Laplacian Smoothing Filter {#laplaciansmoothing}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter implements a simple Laplcian based smoothing filter. In the Laplacian algorithm the &lambda; term has a range of 0 &le; &lambda; &le; 1 and defines a relative distance that a node can move relative to the positions of the nodes neighbors. A &lambda; = 0 value will effectively stop those node types from any movement during the algorithm thus by allowing the user to set this value for specific types of nodes the user can arrest the shrinkage of the surface mesh during the smoothing process.

Currently if you lock the __Default Lambda__ value to Zero (0) the Triple Lines and Quad points will not be able to move because none of their neighbors can move. The user may want to consider allowing a small value of &lambda; for the default nodes which will allow some movement of the triple lines and/or Quad Points. This filter will create additional internal arrays in order to facilitate the calculations. These arrays are

- Float - Lambda values (Same size as Nodes array)
- 64 Bit Integer - Unique Edges Array
- Optinally 8 Bit Integer for Node Type (Same size as Nodes array)
- Integer for Number of Connections for each Node (Same size as Nodes Array)
- 64 Bit Float for Delta Values (3x size of Nodes array)

Due to these array allocations this filter can consume large amounts of memory if the starting mesh is large to start, ie, many nodes. 
The values for the __Node Type__ array can take one of the following values.

    namespace SurfaceMesh {
      namespace NodeType {
        const int8_t Unused = -1;
        const int8_t Default = 2;
        const int8_t TriplePoint = 3;
        const int8_t QuadPoint = 4;
        const int8_t SurfaceDefault = 12;
        const int8_t SurfaceTriplePoint = 13;
        const int8_t SurfaceQuadPoint = 14;
      }
    }

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Iteration Steps | Integer | The number of iteration steps to perform. More steps cause more smoothing but will also cause the volume to shrink more. |
| Default Lambda | Double | The value of lambda to apply to general internal nodes that are not Triple lines, Quad Points or on the "surface" of the volume. |
| Surface Points Lambda | Double | The value of lambda to apply to nodes that lie on the outer surface of the volume |
| Triple Line Lambda | Double | Value of Lambda to apply to nodes designated as Triple Line nodes. |
| Quad Points Lambda | Double | Value of Lambda to apply to nodes designated as Quad points. |
| Surface Triple Line Lambda | Double | Value of Lambda for Triple Lines that lie on the outer surface of the volume |
| Surface Quad Points Lambda | Double | Value of Lambda for the Quad Points that lie on the outer surface of the volume. |

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Vertex Array | SurfaceMeshNodes | The shared list of nodes that make up the mesh |
| Triangle Array | SurfaceMeshTriangles | The List of triangles in the Surface Mesh |
| Node Type | SurfaceMeshNodeType | The array that describes the type of node. (See table in description above) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



