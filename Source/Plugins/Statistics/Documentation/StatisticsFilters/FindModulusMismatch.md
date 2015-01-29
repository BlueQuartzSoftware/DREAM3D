Find Elastic Modulus Mismatch {#findmodulusmismatch}
==========

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter computes the modulus difference between the two grains bounding each surface mesh triangle.

## Parameters ##
None

## Required DataContainers ##
+ Volume
+ SurfaceMesh


## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Face | SurfaceMeshFaceLabels | N x 2 Col of signed integer |  |
| Feature (Volume) | Moduli | One (1) value (float) defining the elastic modulus of the **Feature** | | 


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Face | SurfaceMeshDeltaModulus | float value | absolute value of difference in elastic modulus of bounding grains |

## Authors ##

**Copyright:** 2014 Will Lenthe (UCSB)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.1.0

**License:**  See the License.txt file that came with DREAM3D.


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

