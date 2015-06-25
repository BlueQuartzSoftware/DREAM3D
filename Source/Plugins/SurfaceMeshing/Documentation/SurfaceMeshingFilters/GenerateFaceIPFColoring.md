Generate IPF Colors (Face) {#generatefaceipfcoloring}
============

## Group (Subgroup) ##
Processing (Crystallography)

## Description ##
This Filter generates a pair of colors for each **Triangle** in a **Triangle Geometry** based on the inverse pole figure (IPF) color scheme for the present crystal structure. Each **Triangle** has 2 colors since any **Face** sits at a boundary between 2 **Features** for a well-connected set of **Features** that represent _grains_. The reference direction used for the IPF color generation is the _normal_ of the **Triangle**.

------------

![Face IPF Coloring](GenerateFaceIPFColoring.png)
@image latex GenerateFaceIPFColoring.png "Face IPF Coloring" width=6in

------------

## Parameters ##
None

## Required Geometry ##
Image + Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | FaceLabels | Int | (2) | Specifies which **Features** are on either side of each **Face** |
| Face | FaceNormals | Double | (3) | Specifies the normal of each **Face** |
| Feature | EulerAngles | Float | (3) | Three angles defining the orientation of the **Feature** in Bunge convention (Z-X-Z) |
| Feature | Phases | Int | (1) | Specifies to which phase each **Feature** belongs |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | IPFColors | UInt8 | (6) | A set of two RGB color schemes encoded as unsigned chars for each **Face** |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


