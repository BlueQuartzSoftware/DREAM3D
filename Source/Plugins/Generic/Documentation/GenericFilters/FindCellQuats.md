Find Cell Quaternions {#findcellquats}
=======

## Group (Subgroup) ##
Generic Filters (Crystallography)

## Description ##

This filter determines the _quaternions_ for each **Element**, given the _Euler_ angles for the **Element**.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Element | CellEulerAngles | Float | (1,3) | Euler angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |
| Element | CellPhases | Int | (1,1) | Id specifying the **Ensemble** of the **Element** |
| Ensemble | CrystalStructures | Int | (1,1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Element | Quats | Float | (1,4) | Specifies the orientation of the **Element** in quaternion representation |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

