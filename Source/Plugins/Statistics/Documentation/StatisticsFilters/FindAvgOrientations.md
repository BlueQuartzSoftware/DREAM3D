Find Feature Average Orientations {#findavgorientations}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This Filter determines the average orientation of each **Feature** by the following algorithm:

1. Gather all **Elements** that belong to the **Feature**
2. Using the symmetry operators of the phase of the **Feature**, rotate the quaternion of the **Feature**'s first **Element** into the *Fundamental Zone* nearest to the origin.
3. Rotate each subsequent **Elements**'s quaternion (with same symmetry operators) looking for the quaternion closest to the quaternion selected in Step 2.
4. Average the rotated quaternions for all **Elements** and store as the average for the **Feature**

Note: The process of finding the nearest quaternion in Step 3 is to account for the periodicity of orientation space, which would cause problems in the averaging if all quaternions were forced to be rotated into the same *Fundamental Zone*

Note: The quaternions can be averaged with a simple average because the quaternion space is not distorted like Euler space.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Element** belongs. |
| Element | Quats | Float | (4) | Specifies the orientation of the **Element** in quaternion representation |
| Element | CellPhases | Int | (1) | Specifies the **Ensemble** of the **Element** |
| Ensemble | CrystalStructures | Int | (1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | AvgQuats | Float | (4) | Defines the average orientation of the **Feature** in quaternion representation  (<x,y,z>, w) |
| Feature | FeatureEulerAngles | Float | (3) | Defines the orientation of each **Feature** in Bunge convention (Z-X-Z) |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

