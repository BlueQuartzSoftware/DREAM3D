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


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


