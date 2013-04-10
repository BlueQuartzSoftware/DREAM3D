Change Resolution {#changeresolution}
======

## Group (Subgroup) ##
Sampling Filters (Resolution)

## Description ##
This Filter changes the **Cell** spacing/resolution based on inputs from the user.  The values entered are the desired new resolutions (not multiples of the current resolution).  The number of **Cells** in the volume will likely change when the resolution values are changed and thus the user should be cautious of generating "too many" **Cells** by entering very small values (i.e. very high resolution).  

A new grid of **Cells** is created and "overlaid" on the existing grid of **Cells**.  There is currently no *interpolation* performed, rather the attributes of the old **Cell** that each new **Cell**'s centroid lies within is assigned to that new **Cell**.


## Parameters ##

| Name | Type |
|------|------|
| X Res | Double |
| Y Res | Double |
| Z Res | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



