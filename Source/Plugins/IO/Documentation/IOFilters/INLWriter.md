Write INL File {#inlwriter}
======

## Group (Subgroup) ##

IO Filters (Output)

## Description ##

This filter writes out a file in column format that is space delimited. Header lines are denoted by the "#" character. The columns are the following:

- phi1
- Phi
- phi2
- x Position (Microns)
- y Position (Microns)
- z Position (Microns)
- Feature Id
- Phase Id
- Symmetry

Some information about the phase is included in the header section of the file in addtion
 to values for the Origin, Step Size, Dimensions and number of **Features** in the file. In order to gather this information the filter
 such as "Find Number of Features" needs to be inserted before this filter.

## Parameters ##

| Name | Type |
|------|------|
| Output File | String |

## Required DataContainers ##

Volume Data Container

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | CellEulerAngles | | These are the angles used to determine the colors | Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |
| Cell | CellPhases | | These are used to determine which ensemble the **Cell** belongs to | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | | These are the symmetries of the ensembles, which dictate orientation operations and which color palatte is used | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Ensemble | Number of Features | The number of features per phase | | Find Num. Features (in the Statistics group) |
| Ensemble | Material Name |  | | Read H5Ebsd File (IO) |

## Created Arrays ##
None




## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


