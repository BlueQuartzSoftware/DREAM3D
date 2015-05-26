Quick Surface Mesh {#quicksurfacemesh}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This surface meshing algorithm proceeds by creating a pair of triangles for each face of the **Cell** where the neighboring **Cell** have a different **Feature** id value. No attempt is made to mimic the operations of a MultiMaterial Marching Cubes algorithm. The meshing operation is extremely quick but can result in a surface mesh that is very "stair stepped". The user is encouraged to use a smoothing operation on the surface mesh.



---------------

![Example output showing a single grain on the edge of a box. Note the sharp edge of the box.](QuickSurfaceMeshOutput.png)
@image latex QuickSurfaceMeshOutput.png "Example output showing a single grain on the edge of a box. Note the sharp edge on the box." width=3in

---------------

## Parameters ##
None

## Required DataContainers ##
Voxel
SurfaceMesh - This will create the Topology (Vertices and Triangles) of the surface mesh over writing anything that is currently in the SurfaceMeshDataContainer


## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell (Voxel) | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##
| Type | Default Name | Comment |
|------|--------------|---------|
| Vertex Array | SurfaceMeshNodes | The shared list of nodes that make up the mesh |
| Triangle Array | SurfaceMeshTriangles | The List of triangles in the Surface Mesh |
| Vertex | SurfaceMeshNodeType | N x 1 Col of unsigned char |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


