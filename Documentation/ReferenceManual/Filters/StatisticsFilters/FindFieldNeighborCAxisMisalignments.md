Find Field Neighbor C-Axis Misalignments {#findfieldneighborcaxismisalignments}
==========

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter determines, for each **Field**, the c-axis misalignments with the **Fields** that are in contact with it.  The c-axis misalignments are stored as a list (for each **Field**) of angles.

## Parameters ##

| Name | Type |
|-------|------|
|Find Average C-Axis Mialignment | Boolean |  

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Field | NeighborLists | List of the contiguous neighboring **Fields** for a given **Field** |  | Find Field Neighbors (Statistics) |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. | Find Field Average Orientations (Statistics) |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| | Find Field Phases (Generic), Read Field Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | CAxisMisalignmentList | List of the c-axis misalignment angles with the contiguous neighboring **Fields** for a given **Field** |  |  
| Field | AvgCAxisMisalignments | Number weighted average of neighbor c-axis misalignments. | Only created if Find Average C-Axis Misalignments IS checked. |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

