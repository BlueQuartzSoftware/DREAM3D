Generate Rodrigues Colors {#generaterodriguescolors}
=============

## Group (Subgroup) ##

Generic Filters (Misc)

## Description ##

This filter will generate Rodrigues colors for each voxel.


## Input Options ##

| Option | Type |
|-------|-------|
| X Reference Direction | Double |
| Y Reference Direction | Double |
| Z Reference Direction | Double |


## Required DataContainers ##

Voxel DataContainer

## Required Arrays ##

| Type | Default Name | Description  | Filters Known to Create Data |
|------|--------------|------------|-----|
| Cell | CellEulerAngles |  These are the angles used to determine the colors | Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |
| Cell | CellPhases |  These are used to determine which ensemble the **Cell** belongs to | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures |  These are the symmetries of the ensembles, which dictate orientation operations and which color palatte is used | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Name | Description |
|------|------|-------------|
| Cell | IPFColors | The RGB colors are encoded as an unsigned char triplet  |


### Authors ###


**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:** See the License.txt file that came with DREAM3D.

See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

