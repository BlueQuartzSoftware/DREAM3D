Quick Surface Mesh {#quicksurfacemesh}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This surface meshing algorithm proceeds by creating a pair of triangles for each face of the **Cell** where the neighboring **Cell** have a different **Field** id value. No attempt is made to mimic the operations of a MultiMaterial Marching Cubes algorithm. The meshing operation is extremely quick but can result in a surface mesh that is very "stair stepped". The user is encouraged to use a smoothing operation on the surface mesh.
 

## Parameters ##
None

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | GrainIds |  |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
|  | Nodes | SurfaceMeshDataContainer Property |
|  | Triangles | SurfaceMeshDataContainer Property |
| char | Node Type | The Type of Nodes. One value for each Node created. |



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.

