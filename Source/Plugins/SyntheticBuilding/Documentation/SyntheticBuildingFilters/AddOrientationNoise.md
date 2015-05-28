Add Orientation Noise {#addorientationnoise}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Crystallography)

## Description ##
This filter adds "noise" to the **Cell** orienations.  This filter is intended to add "realism" to synthetic structures that have single orientations assigned to **Features** (an in turn, all of the **Cells** they own).  The user can specify a magnitude of the "noise", which refers to the maximum rotation angle that is applied to the existing **Cells**.  The algorithm for applying the orientation "noise" is as follows:

1. Obtain the orientation of the **Cell**.
2. Generate a random number between *0* and the user defined magnitude. This value is the rotation angle.
3. Generate a random unit vector.  This is the rotation axis.
4. Calculate the rotation corresponding to the axis-angle pair generated in Steps 2-3 and apply it to the orientation of the **Cell** to obtain a new orientation.
5. Repeat for all **Cells**



## Parameters ##

| Name | Type |
|------|------|
| Magnitude of Orientation Noise | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |

## Created Arrays ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


