Verify Triangle Winding {#verifytrianglewinding}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter analyzes the mesh for consistent triangle winding and fixes any inconsistencies that are found. This algorithm can be memory intensive for larger meshes. The algorithm is an advancing wave front type of algorithm. The algorithm is know to have problems if there are disconnected grains with the same ID in the surface mesh.


## Parameters ##
None

## Required Arrays ##

| Vertex Array | SurfaceMeshNodes | The shared list of nodes that make up the mesh |
| Triangle Array | SurfaceMeshTriangles | The List of triangles in the Surface Mesh |

## Created Arrays ##
None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



