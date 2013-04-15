Generate IPF Colors {#generateipfcolors}
========

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This filter will generate Inverse Pole Figure colors for Cubic or Hexagonal Crystal Structures. The user can enter the Reference direction and is defaulted to [001].

![IPF Color Triangle](IPFFilterLegend.png)
@image latex IPFFilterLegend.png

## Input Options ##

| Option | Type |
|-------|-------|
| X Reference Direction | Double |
| Y Reference Direction | Double |
| Z Reference Direction | Double |


## Required DataContainers ##
Voxel

## Required Arrays ##


| Type | Name | Desription | Comment |
|------|------|---------|-------|
| Cell | CellEulerAngles | | These are the angles used to determine the colors |
| Cell | CellPhases | | These are used to determine which ensemble the **Cell** belongs to |
| Ensemble | CrystalStructures | | These are the symmetries of the ensembles, which dictate orientation operations and which color palatte is used |

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | IPFColors | | The RGB colors are encoded as an unsigned char triplet  |

### Authors ##
**Copyright:** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:** See the License.txt file that came with DREAM3D.
