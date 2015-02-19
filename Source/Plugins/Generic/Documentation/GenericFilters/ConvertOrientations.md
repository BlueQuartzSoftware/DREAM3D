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

| Type | Default Name | Description |
|----------|-------------------|-------------|
| Element     | Varies            | 3 (Euler Angles, Rodrigues Vectors) or 4 (Quaternions, Axis-Angle Pairs) values (floats) defining the orientation of the **Element** |
| Element     | EnsembleId            | Ensemble Id (int) specifying the phase of the **Cell** |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble (Hexagonal=0, Cubic=1, Orthorhombic=2) |
## Created Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Element | Varies             | 3 (Euler Angles, Rodrigues Vectors) or 4 (Quaternions, Axis-Angle Pairs) values (floats) defining the orientation of the **Element** |

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

