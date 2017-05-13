
## Surface Mesh Definitions
Creating a surface mesh in DREAM3D can be accomplished in several ways. There are a pair of filters [Multi-Material Marching Cubes (Slice at a Time)](#m3cslicebyslice) and [Quick Surface Mesh](#quicksurfacemesh) that will generate a triangular based mesh of the primary **Feature** for you. The terminology of the underlying data structures is something that the user should understand so that they can be well informed. We start out by defining a triangle **T**  which we will call a  **Face**. Each Triangle has 3 **Vertex** (V0, V1, V2) which are  referred to as a **Point**, **Vertex** and/or **Node** interchangeably within DREAM3D.

![Triangle or Face](Images/TriangleSmall.png)
@image latex TriangleSmall.png " " width=3in


These data structures are declared in DREAM3D in the following code:

    typedef struct
    {
        Float_t pos[3];
    } Vert_t;

    typedef struct
    {
        signed int verts[3];
    } Face_t;

DREAM3D stores all the vertices in a single array. DREAM3D has the notion of a "shared vertex list" where each vertex is only listed once in the vertex list. Each Triangle is then created by referencing 3 indexes out of the vertex list to form the 3 points of the triangle.

## Vertex & Triangle Attributes
DREAM3D filters can attach _attributes_ to each vertex or triangle. These attributes can be _scalar_ or _vector_ of any numerical type. Some attributes are dependent on the winding of the triangle. From OpenGL the normal is computed via a right-hand-rule and so proper triangle winding is important when visualizing the mesh and also is important to some filters such as the [Grain Face Curvature Filter](#grainfacecurvaturefilter). This is why there are filters such as [Reverse Triangle Winding](#reversetrianglewinding) and [Verify Triangle Winding](#verifytrianglewinding) in order to ensure proper winding and normal calculations.

## Surface Mesh Data Container
To support the creation and manipulation of a surface mesh DREAM3D introduced a new data container type called the ``SurfaceMeshDataContainer`` and have as its core properties the shared vertex array and triangle array. These arrays have default names given to them which are defined in ``DREAM3DLib/Common/Constants.h``

    const std::string SurfaceMeshNodes("SurfaceMeshNodes");
    const std::string SurfaceMeshTriangles("SurfaceMeshTriangles");

In the filter documentation the following terminology will be used:

+ Vertex Array: Refers to the shared vertex array stored in the ``SurfaceMeshDataContainer``
+ Triangle Array:  Refers to the triangle array stored in the ``SurfaceMeshDataContainer``
+ Edge Array: Refers to the edge array stored in the ``SurfaceMeshDataContainer``


## Edge Data
 Update with algorithm to generate Unique Edge Ids

## Filter List ##

