Find Twin Boundaries {#findcoherenttwinnomals}
==========

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter identifies the coherent plane normal for all **Faces** that have a Sigma 3 twin relationship.  The filter uses the average orientation of the **Features** on either side of the **Face** to determine the shared <111> direction between the **Features**.  The shared axis in both crystal frames is rotated to the orientation reference frame and averaged.

## Parameters ##
None

## Required DataContainers ##
+ Volume
+ SurfaceMesh


## Required Arrays ##

| Type | Default Name | Description |
|------|--------------|-------------|
| Face | SurfaceMeshFaceLabels | N x 2 Col of signed integer |
| Face | SurfaceMeshTwinBoundary | boolean value equal to 1 for twin and 0 for non-twin |
| Feature (Volume) | AvgQuats | Four (4) values (floats) defining the average orientation of the **Feature** in quaternion representation |
| Feature (Volume) | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**|
| Ensemble (Volume) | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase |

## Created Arrays ##

| Type | Default Name | Description |
|------|--------------|-------------|
| Face | SurfaceMeshFaceNormals | Direction of coherent plane normal in the orientation reference frame |

## Authors ##

**Copyright:** 2015 William C. Lenthe (UCSB)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0

**License:**  See the License.txt file that came with DREAM3D.