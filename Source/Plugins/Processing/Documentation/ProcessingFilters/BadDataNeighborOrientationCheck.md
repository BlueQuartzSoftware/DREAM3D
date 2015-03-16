Bad Data Orientation Correlation {#baddataneighbororientationcheck}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter compares the orientations of "*bad*" **Cells** with their neighbor **Cells**.  If the misorientation is below a user defined tolerance for a user defined number of neighbor **Cells** , then the "*bad*" **Cell** will be changed to a "*good*" **Cell**.

*Note:* only the boolean value defining the **Cell** as "good" or "bad" is changed, not the data at **Cell**.

*Note:* the filter will iteratively reduce the required number of neighbors from 6 until it reaches the user defined number. So, if the user selects a required number of neighbors of 4, then the filter will run with a required number of neighbors of 6, then 5, then 4 before finishing.  

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Misorientation Tolerance (Degrees) | Float | Angular tolerance used to compare with neighboring **Cells** |
| Required Number of Neighbors | Integer | Minimum number of neighbor **Cells** that must have orientations within above tolerace to allow **Cell** to be changed |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | Quats | Float | (4) | Specifies the orientation of the **Cell** in quaternion representation |
| Cell | GoodVoxels | Boolean | (1) | Used to define **Cells** as "good" or "bad"  |
| Cell | CellPhases | Int | (1) | Specifies the **Ensemble** of the **Cell** |
| Ensemble | CrystalStructures | Int | (1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

