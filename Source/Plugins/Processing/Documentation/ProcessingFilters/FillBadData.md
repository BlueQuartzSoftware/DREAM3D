Fill Bad Data {#fillbaddata}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter removes small "*noise*" in the data, but keeps larger regions that are possibly features, e.g., pores or defects.
This Filter collects the *bad* **Cells** (*GrainId = 0*) and runs the _erode_ operation from the [Erode/Dilate Bad Data Filter](OpenCloseBadData.html "") until none remain. However, contiguous groups of *bad* **Cells** that have at least as many **Cells** as the minimum defect size enter by the user will not be *eroded*.

## Parameters ##

| Name | Type |
|------|------|
| Minimum Allowed Defect Size | Integer |
| Replace Bad Data | Boolean |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

