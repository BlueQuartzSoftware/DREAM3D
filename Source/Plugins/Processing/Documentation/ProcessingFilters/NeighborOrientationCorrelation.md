Neighbor Orientation Correlation {#neighbororientationcorrelation}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter first identifies all **Cells** that have a *confidence index* below the minimum set by the user.  Then, for each of those **Cells**, their neighboring **Cells** are checked to determine the number of neighbor **Cells** with orientations different than the reference **Cell** by more than the user defined *misorientation tolerance*.  In addition the neighboring **Cells** are compared with each other to determine the number of neighboring **Cells** that have the same orientation (again within the user defined tolerance). 
The *Cleanup Level* set by the user is then used to determine which **Cells** are replaced.  If a level of 5 is set, then at least 5 of the neighboring **Cells** must be different than the reference **Cell** and at least 5 of the neighboring **Cells** must be the same as each other (and so on for other *Cleanup Level*). 
If a **Cell** meets the replacement criteria, then all of its attributes are replaced with the attribtues of one of the neighboring **Cells** that are the same as each other.

*Note:* the filter will iteratively reduce the *Cleanup Level* from 6 until it reaches the user defined number. So, if the user selects a level of 4, then the filter will run with a level of 6, then 5, then 4 before finishing.  

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Minimum Confidence Index | Float | Sets the minimum value of 'confidence' a **Cell** must have |
| Misorientation Tolerance (Degrees) | Float | Angular tolerance used to compare with neighboring **Cells** |
| Cleanup Level | Int | Minimum number of neighbor **Cells** that must have orientations within above tolerace to allow **Cell** to be changed | 

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | Quats | Float | (1,4) | Specifies the orientation of the **Cell** in quaternion representation |
| Cell | Confidence Index | Float | (1,1) | Specifies the confidence in the orientation of the **Cell** (TSL data) |
| Cell | CellPhases | Int | (1,1) | Specifies the **Ensemble** of the **Cell** |
| Ensemble | CrystalStructures | Int | (1,1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

