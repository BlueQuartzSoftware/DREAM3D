Jumble Orientations {#jumbleorientations}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Crystallography)

## Description ##
This filter "jumbles" the orientations of the **Features** amongst each other.  The filter cycles through all the **Features**, randomly choosing another **Feature** to swap orientations with.  On each swap, it is checked that the randomly selected **Feature** is of the same phase.

Note: by cycling through the **Features**, it is garaunteed that each **Feature** will be swapped at least once, but it could actually be swapped multiple times if a later **Feature** randomly selects it to swap with.


## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellEulerAngles |  |
| Feature | AvgQuats |  |
| Feature | FeatureEulerAngles |  |

##Authors##






## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


