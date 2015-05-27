Find Kernel Average Misorientations {#findkernelavgmisorientations}
======

## Group (Subgroup) ##
Orientation Analysis Filters (Statistics)

## Description ##
This filter determines the Kernel Average Misorientation (KAM) for each **Cell**.  The user can select the size of the kernel to be used in the calculation.  The kernel size entered by the user is the *radius* of the kernel (i.e. entering values of *1*, *2*, *3* will result in a kernel that is *3*, *5*, and *7* **Cells** in size in the x, y and z directions, respectively).  The algorithm for determination of KAM is as follows:

1. Calculate the misorientation angle between each **Cell** in a kernel and the central **Cell** of the kernel.
2. Average all of the misorientations for the kernel and store at the central **Cell**.

*Note that all **Cells** in the kernel are weighted equally during the averaging, though they are not equidistant from the central **Cell**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Kernel Radius | Int (x3) | Size of the kernel in the x, y, and z directions (in number of **Cells**) |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Cell | Quats | Float | (4) | Specifies the orientation of the **Cell** in quaternion representation |
| Cell | CellPhases | Int | (1) | Specifies the **Ensemble** of the **Cell** |
| Ensemble | CrystalStructures | Int | (1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | KernelAverageMisorientations | Float | (1) | Average misorientation for all **Cells** within the kernel and the central **Cell** |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


