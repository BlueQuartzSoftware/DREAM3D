Find Feature Reference Misorientations {#findfeaturereferencemisorientations}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter calculates the misorientation angle between each **Cell** within a **Feature** and a *reference orientation* for that **Feature**.  The user can choose the *reference orientation* to be used for the **Features** from a drop-down menu.  The options for the *reference orientation* are the average orientation of the **Feature** or the orientation of the **Cell** that is furthest from the *boundary* of the **Feature**.

Note: the average orientation of the **Feature** is a typical choice, but if the **Feature** has undergone plastic deformation and the amount of lattice rotation developed is of interest, then it may be more reasonable to use the orientation *near the center* of the **Feature** as it may not have rotated and thus serve as a better *reference orientation*.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Reference Orientation | Choice | Specifies the *reference orientation* to use when comparing to each **Cell** |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Element | Quats | Float | (4) | Specifies the orientation of the **Element** in quaternion representation |
| Element     | Phases            | Int | (1) | Specifies the **Ensemble** of the **Cell** |
| Ensemble | CrystalStructures | Int | (1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |
| Feature | AvgQuats | Float | (4) | Defines the average orientation of the **Feature** in quaternion representation  (<x,y,z>, w). Only required if the *reference orientation* is selected to be the average of the **Feature** |
| Element | GBEuclideanDistances | Float | (1) | Distance the **Elements** are from the *boundary* of the **Feature** they belong to. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureReferenceMisorientations | Float | (1) | Misorientation angle (in degrees) between **Element**'s orientation and the reference orientation of the **Feature** that owns that **Element** |
| Feature | FeatureAvgMisorientations | Float | (1) | Average of the *FeatureReferenceMisorientation* values for all of the **Elements** that belong to the **Feature** |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

