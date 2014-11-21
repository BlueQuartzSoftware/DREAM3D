Find Feature Sizes {#findsizes}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the sizes of all **Features**.  The filter simply iterates through all **Cells** asking the for **Feature** that owns them.  
The count of **Cells** for each **Feature** is then stored as *NumCells* and the *Volume* and *EquivalentDiameter* are also calculated (and stored) by knowing 
the volume of each **Cell**.

## Parameters ##

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Feature**. |  |
| Feature | NumCells | Number (int) of **Cells** that are owned by the **Feature**. | This value does not place any distinction between **Cells** that may be of a different size. |
| Feature | Volumes | Volume (float) in um^3 of the **Feature**. | This value may be "redundant" from the NumCells value if all **Cells** are the same size and is calculable from the EquivalentDiameters value. |

## Authors ##

**Copyright:** 2014 Michael A. Groeber (AFRL), 2014 Michael A. Jackson (BlueQuartz Software), 2014 Joseph C. Tucker (UES, Inc.)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.1.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

