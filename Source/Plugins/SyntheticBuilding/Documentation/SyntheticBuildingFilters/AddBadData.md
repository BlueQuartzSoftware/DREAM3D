
Add Bad Data {#addbaddata}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Misc)

## Description ##
This filter adds "bad" data to a volume.  This filter is intended to add "realism" (i.e. more representative of an experimental dataset) to synthetic structures that don't have any "bad" **Cells**.  The user can choose to add "random noise" and/or "noise" along **Feature** boundaries.  If they choose to add either kind of "noise", the user must then set the volume fraction of **Cells** to set as "bad".  The volume fractions entered by the user apply to only the set of **Cells** that the "noise" would affect.  For example, if the user chose *0.2* for the volume fraction of boundary "noise", then each boundary **Cell** would have a *20%* chance of being changed to a "bad" **Cell** and all other **Cells** would have a *0%* chance of being changed.


## Parameters ##

| Name | Type |
|------|------|
| Add Random Noise | Boolean |
| Volume Fraction Random Noise | float |
| Add Boundary Noise | Boolean |
| Volume Fraction Boundary Noise | float |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GBEuclideanDistances |  |  | Find Euclidean Distance Map (Statistics) |

## Created Arrays ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


