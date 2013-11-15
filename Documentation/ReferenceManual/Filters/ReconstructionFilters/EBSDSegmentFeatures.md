Segment Features (Misorientation) {#ebsdsegmentfeatures}
======

## Group (Subgroup) ##
Reconstruction Filters (Segmentation)

## Description ##
This Filter segments the **Features** by grouping neighboring **Cells** that satisfy the _misorientation tolerance_, i.e., have _misorientation angle_ less than the value set by the user. The __Cell__  _GrainIds_ established by this filter are used by the other Filters.


## Parameters ##

| Name | Type | Comment |
|------|------|------|
| Misorientation Tolerance | Double | Value is in degress |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  | Single Threshold (Cell Data) (Processing), Multi Threshold (Cell Data) (Processing) |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. | Find Cell Quaternions (Generic) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. |  |
| Feature | Active | Boolean value specifying if the **Feature** is still in the sample (1 if the **Feature** is in the sample and 0 if it is not) | At the end of the filter, all **Features** will be "Active" as the "Inactive" **Features** will have been removed.  |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

