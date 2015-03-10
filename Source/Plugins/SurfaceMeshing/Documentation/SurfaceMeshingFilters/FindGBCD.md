Find GBCD {#findgbcd}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter computes the Grain Boundary Character Distribution (GBCD).


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Face   | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Face   | SurfaceMeshFaceAreas | |  | Generate Triangle Areas (SurfaceMeshing) |
| Face   | SurfaceMeshFaceNormals | N x 3 Col of floats |  | Generate Triangle Normals Filter (SurfaceMeshing) |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Ensemble | GBCD | |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

