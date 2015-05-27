Crop Volume {#cropvolume}
=============

## Group (Subgroup) ##
Sampling Filters (Cropping/Cutting)

## Description ##
This Filter allows the user to crop a volume of interest.  The input parameters are in units of **Cells**.  For example, if a volume was 100x100x100 **Cells** and each **Cell** was 0.25x0.25x0.25 microns, then if the user wanted to crop the last 5 microns in the X direction, then the user would enter the following:

Xmin = 80,
Xmax = 99,
Ymin = 0,
Ymax = 99,
Zmin = 0,
Zmax = 99

*Note:* The input parameters are inclusive and begin at *0*, so in the above example *0-99* covers the entire range of **Cells** in a given dimension.

See also the tutorial page [here](tutorialcroppingdata.html).

It is possible with this Filter to fully remove **Features** from the volume, possibly resulting in consistency errors if more Filters process the data in the pipeline. If the user selects to _Renumber Features_ then the *Feature Ids* array will be adjusted so that all **Features** are continuously numbered starting from 1. The user should decide if they would like their **Features** renumbered or left alone (in the case where the cropped output is being compared to some larger volume).

The user has the option to save the cropped volume as a new **Data Container** or overwrite the current volume.

Normally this Filter will leave the origin of the volume set at (0,0,0), which means output files like the XDMF file will have the same (0,0,0) origin. When viewing both the original larger volume and the new cropped volume simultaneously the cropped volume and the original volume will have the same origin which makes the cropped volume look like it was shifted in space. In order to keep the cropped volume at the same absolute position in space the user should turn **ON** the _Update Origin_ check box.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| X Min | Int | Lower X (column) bound of the volume to crop out |
| Y Min | Int | Lower Y (row) bound of the volume to crop out |
| Z Min | Int | Lower Z (plane) bound of the volume to crop out |
| X Max | Int | Upper X (column) bound of the volume to crop out |
| Y Max | Int | Upper Y (row) bound of the volume to crop out |
| Z Max | Int | Upper Z (plane) bound of the volume to crop out |
| Save as New DataContainer | Boolean | Specifies if the new grid of **Cells** should replace the current **Geometry** or if a new **Data Container** should be created to hold it |
| Renumber Features | Boolean | Specifies if the **Features** should be renumbered if some **Features** 'disappear' when changing the resolution (ie coarsening the resolution may cause small **Features** to be removed. |
| Update Origin | Boolean | Specifies whether the origin of the cropped volume should be updated to the absolute location in the original volumes reference frame (true) or whether the origin of the original volume should be used as the origin of the cropped volume (false) |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. (Only required if Renumber Features is *true*) |

## Created Arrays ##
None

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


