Find Field Sizes {#findsizes}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the sizes of all **Fields**.  The filter simply iterates through all **Cells** asking the for **Field** that owns them.  
The count of **Cells** for each **Field** is then stored as *NumCells* and the *Volume* and *EquivalentDiameter* are also calculated (and stored) by knowing 
the volume of each **Cell**.

## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Field**. |  |
| Field | NumCells | Number (int) of **Cells** that are owned by the **Field**. | This value does not place any distinction between **Cells** that may be of a different size. |
| Field | Volumes | Volume (float) in um^3 of the **Field**. | This value may be "redundant" from the NumCells value if all **Cells** are the same size and is calculable from the EquivalentDiameters value. |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



