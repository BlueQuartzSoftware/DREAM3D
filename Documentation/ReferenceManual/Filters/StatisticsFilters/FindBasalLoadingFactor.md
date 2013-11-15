Find Basal Loading Factor {#findbasalloadingfactor}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter calculates the Basal Loading Factor of each **Feature** given its average orientation and a user defined loading axis.  The Basal Loading Factor is the stress on the basal plane in the given loading direction.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Loading X: | Double | X component of the loading axis in the sample reference frame |
| Loading Y: | Double | Y component of the loading axis in the sample reference frame |
| Loading Z: | Double | Z component of the loading axis in the sample reference frame |

## Required DataContainers ##

Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Feature | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | BasalLoadingFactor | Value of the Basal Loading Factor (float) the stress on the basal plane for the given loading direction. |

## Authors ##

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

