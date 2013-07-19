Minimum Number of Neighbors Filter {#minneighbors}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter sets the minimum number (an integer) of contiguous neighboring **Fields** a **Field** must have to remain in the structure. Entering zero results in nothing changing.  Entering a number larger than the maximum number of neighbors of any **Field** generates an _error_ (since all **Fields** would be removed). The user needs to proceed conservatively here when choosing the value for the minimum to avoid accidentally exceeding the maximum.

After **Fields** are removed for not having enough neighbors, the remaining **Fields** are _coarsened_ iteratively, one **Cell** per iteration, until the gaps left by the removed **Fields** are filled.  Effectively, this is an isotropic grain growth in the regions around removed **Fields**.

## Parameters ##

| Name | Type |
|------|------|
| Minimum Number Neighbors | Integer |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Field | NumNeighbors | Value (int) equal to the number of contiguous neighboring **Fields** for a given **Field** |  | Find Field Neighbors (Statistics) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

