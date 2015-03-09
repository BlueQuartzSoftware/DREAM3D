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

See also the tutorial page [here](tutorialcroppingdata.html).

It is possible with this filter to fully remove grains from the voxel volume resulting in consistency errors if more filters process the data in the pipeline. If the user selects to _Renumber Grains_ then the **GrainIds** array will be adjusted so that all grains are continuously numbered starting from 1. The user should decide if they would like their grain ids renumbered or left alone (in the case where the cropped output is being compared to some larger volume).

Normally this filter will leave the origin of the volume set at (0, 0, 0) which means output files like the XDMF file will have the same (0,0,0) origin. When viewing both the original larger volume and the new cropped volume simultaneously the cropped volume and the original volume will have the same origin which makes the cropped volume look like it was shifted in space. In order to keep the cropped volume at the same absolute position in space the user should turn **ON** the _Update Origin_ check box.

## Parameters ##

| Name | Type |
|------|------|
| X Min | Integer |
| Y Min | Integer |
| Z Min | Integer |
| X Max | Integer |
| Y Max | Integer |
| Z Max | Integer |
| Renumber Grains | Boolean |
| Update Origin | Boolean |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

