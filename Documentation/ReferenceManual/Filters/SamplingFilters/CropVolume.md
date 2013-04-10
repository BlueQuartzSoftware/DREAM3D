Crop Volume {#cropvolume}
======

## Group (Subgroup) ##
Sampling Filters (Cropping/Cutting)

## Description ##
This Filter allows the user to crop a volume of interest.  The input parameters are in units of **Cells**.  For example, if a volume was 100x100x100 **Cells** and each **Cell** was 0.25x0.25x0.25 microns, then if the user wanted to crop the last 5 microns in the X-direction, then the user would enter the following:

Xmin = 80,
Xmax = 99,
Ymin = 0,
Ymax = 99,
Zmin = 0,
Zmax = 99

Note: the input parameters are inclusive and begin at *0*, so in the above example *0-99* covers the entire range of **Cells** in a given dimension.


## Parameters ##

| Name | Type |
|------|------|
| X Min | Integer |
| Y Min | Integer |
| Z Min | Integer |
| X Max | Integer |
| Y Max | Integer |
| Z Max | Integer |

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



