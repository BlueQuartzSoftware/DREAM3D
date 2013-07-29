Find MDF {#findmdf}
======

## Group (Subgroup) ##
Statistics Filters


To generate a Misorientation Distribution Function (MDF), this filter uses __Field AvgQuats, FieldPhases__, and __SurfaceFields__ and __Ensemble CrystalStructures__, and __TotalSurfaceAreas__ values.


## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |
| Field | SurfaceFields | Boolean flag of 1 if **Field** touches an outer surface or of 0 if it does not | Filter will determine if **Fields** intersect an outer surface if not already determined |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | TotalSurfaceAreas |  |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

