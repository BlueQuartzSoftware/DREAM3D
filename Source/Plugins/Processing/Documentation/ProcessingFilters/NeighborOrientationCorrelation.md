Neighbor Orientation Correlation {#neighbororientationcorrelation}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter first identifies all **Cells** that have a *confidence index* below the minimum set by the user.  Then, for each of those **Cells**, their neighboring **Cells** are checked to determine the number of neighbor **Cells** with orientations different than the reference **Cell** by more than the user defined *misorientation tolerance*.  In addition the neighboring **Cells** are compared with each other to determine the number of neighboring **Cells** that have the same orientation (again within the user defined tolerance). The *Cleanup Level* set by the user is then used to determine which **Cells** are replaced.  If a level of 5 is set, then at least 5 of the neighboring **Cells** must be different than the reference **Cell** and at least 5 of the neighboring **Cells** must be the same as each other (and so on for other *Cleanup Level*).

*Note:* the filter will iteratively reduce the *Cleanup Level* from 6 until it reaches the user defined number. So, if the user selects a level of 4, then the filter will run with a level of 6, then 5, then 4 before finishing.  

## Parameters ##

| Name | Type |
|------|------|
| Minimum Confidence Index | Float |
| Misorientation Tolerance (Degrees) | Float |
| Cleanup Level | Integer |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. | Find Cell Quaternions (Generic) |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | Confidence Index | Scalar value defining the confidence in the orientation of the **Cell** (TSL data) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

