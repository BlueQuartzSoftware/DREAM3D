Find Directional Moduli {#finddirectionalmoduli}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter calculates the directional elastic modulus for each **Feature** given its average orientation, a user defined loading axis, and single crystal compliance values. To compute the directional modulus the compliance matrix is rotated to align it's x axis with the loading direction (assuming that the sample & quaternion reference frames are aligned). The directional modulus is 1/s11 (in the rotated compliance matrix).

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Loading X: | Double | X component of the loading axis in the sample reference frame |
| Loading Y: | Double | Y component of the loading axis in the sample reference frame |
| Loading Z: | Double | Z component of the loading axis in the sample reference frame |

## Required DataContainers ##
Voxel

## Required Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature** | | 
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute.|
| Feature | AvgQuats | Four (4) values (floats) defining the average orientation of the **Feature** in quaternion representation | |
| Ensemble | CrystalCompliances | 6x6 matrix specifying the elastic compliances of the **Phase** | in pascals^-1 |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | DirectionalModuli | Elastic modulus (float) in the specified loading direction | in pascals |

## Authors ##

**Copyright:** 2014 Will Lenthe (UCSB)

**Contact Info:** willlenthe@gmail.com

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


