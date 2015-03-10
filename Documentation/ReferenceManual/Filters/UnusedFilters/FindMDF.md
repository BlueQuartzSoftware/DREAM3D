Find MDF {#findmdf}
======

## Group (Subgroup) ##
Statistics Filters


To generate a Misorientation Distribution Function (MDF), this filter uses __Feature AvgQuats, FeaturePhases__, and __SurfaceFeatures__ and __Ensemble CrystalStructures__, and __TotalSurfaceAreas__ values.


## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature** | Filter will determine **Feature** phases if not previously determined |
| Feature | SurfaceFeatures | Boolean flag of 1 if **Feature** touches an outer surface or of 0 if it does not | Filter will determine if **Features** intersect an outer surface if not already determined |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | TotalSurfaceAreas |  |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

