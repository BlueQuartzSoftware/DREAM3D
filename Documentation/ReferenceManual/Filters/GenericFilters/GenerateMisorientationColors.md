Generate Misorientation Colors {#generatemisorientationcolors}
========

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This filter will generate colors based on the method developed by C. Schuh and S. Patala for Cubic, Hexagonal, Orthorhombic, or Tetragonal Crystal Structures (currently only cubic high is working). Orientations are colored by disorientation relative to a reference orientation. The user can enter an reference orientation (relative to the sample frame). The defualt reference orientation is the sample frame (misorientation from sample frame: 0 degrees about [001]). The user is required to run a filter that will determine if a voxel should have it's color calculated by generating the _GoodVoxels_ array or an equivalent **boolean** array. Typically the **MultiThreshold Cells** or **Single Threshold Cells** filter is run _before_ this filter with an output array set to _GoodVoxels_.


## Input Options ##

| Option | Type |
|-------|-------|
| x Reference Frame Misorientation Axis | Double |
| y Reference Frame Misorientation Axis | Double |
| z Reference Frame Misorientation Axis | Double |
| w Reference Frame Misorientation Angle| Double |


## Required DataContainers ##

Voxel

## Required Arrays ##

| Type | Default Name | Description  | Filters Known to Create Data |
|------|--------------|------------|-----|
| Cell | CellQuats |  These are the orientations used to determine the colors | Find Cell Quaternions |
| Cell | CellPhases |  These are used to determine which ensemble the **Cell** belongs to | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |  
| Cell | GoodVoxels | This is the boolean array that determines if a voxel will have its IPF Color generated or not. | [MultiThreshold Cells](multithresholdcells.html), [Single Threshold Cells](singlethresholdcells.html) |
| Ensemble | CrystalStructures |  These are the symmetries of the ensembles, which dictate orientation operations and which color palatte is used | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##

| Type | Name | Description |
|------|------|-------------|
| Cell | MisorientationColors | The RGB colors are encoded as an unsigned char triplet  |


### Authors ###


**Copyright:** 2013 William Lenthe (University of California Santa Barbara)

**Contact Info:** willlenthe@gmail.com

**Version:** 1.0.0

**License:** See the License.txt file that came with DREAM3D.

See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

