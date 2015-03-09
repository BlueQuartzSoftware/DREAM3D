Convert Orientations {#convertorientations}
=======

## Group (Subgroup) ##
Generic Filters (Crystallography)

## Description ##

This filter determines the _Output Orientation Representation_ for each **Element**, given the _Input Orientation Representation_ for the **Element**.

## Parameters ##
| Name             | Type | Description |
|------------------|------|---------|
| Input Orientation Representation | String | Euler Angles, Quaternions, Rodrigues Vectors, or Axis-Angle pairs |
| Output Orientation Representation | String | Euler Angles, Quaternions, Rodrigues Vectors, or Axis-Angle pairs |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Element     | Varies            | Float | (1,3) or (1,4) | Euler Angles, Rodrigues Vectors, Quaternions or Axis-Angle Pair values defining the orientation of the **Element** |
| Element     | CellPhases            | Int | (1,1) | Specifies the **Ensemble** of the **Cell** |
| Ensemble | CrystalStructures | Int | (1,1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Element     | Varies            | Float | (1,3) or (1,4) | Euler Angles, Rodrigues Vectors, Quaternions or Axis-Angle Pair values defining the orientation of the **Element** |

## Default Array Names ##
| Orientation Representation | Default Name     |
|----------------------------|------------------|
| Euler Angles               | EulerAngles      |
| Quaternions                | Quats            |
| Rodrigues Vectors          | RodriguesVectors |
| Axis-Angle Pairs           | AxisAngles       |


## Authors ##

**Copyright:** 2014 Will Lenthe (UCSB)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

