Find Feature Largest Cross-Sections {#findlargestcrosssections}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the largest cross-sectional area perpendicular to a user-defined direction for all **Features**.  The filter simply iterates through all **Cells** (on each section) asking the for **Feature** that owns them.  
The count of **Cells** for each **Feature** is then converted to an area and stored as the *LargestCrossSection*.  This is done for all sections (only replacing the value of LargestCrossSection if it smaller than the area for the current section).

## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | LargestCrossSection | Area (float) of largest cross-section for **Feature** perpendicular to specified direction. |  |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

