Crop Geometry (Image) {#cropimagegeometry}
=============

## Group (Subgroup) ##
Sampling (Cropping/Cutting)

## Description ##
This **Filter** allows the user to crop an **Image Geometry** of interest.  The input parameters are in units of **Cells**.  For example, if a **Image Geometry** was 100x100x100 **Cells** and each **Cell** was 0.25 x 0.25 x 0.25 units of resolution, then if the user wanted to crop the last 5 microns in the X direction, then the user would enter the following:

Xmin = 80,
Xmax = 99,
Ymin = 0,
Ymax = 99,
Zmin = 0,
Zmax = 99

*Note:* The input parameters are _inclusive_ and begin at *0*, so in the above example *0-99* covers the entire range of **Cells** in a given dimension.

See also the tutorial page [here](tutorialcroppingdata.html).

It is possible with this **Filter** to fully remove **Features** from the volume, possibly resulting in consistency errors if more **Filters** process the data in the pipeline. If the user selects to _Renumber Features_ then the *Feature Ids* array will be adjusted so that all **Features** are continuously numbered starting from 1. The user should decide if they would like their **Features** renumbered or left alone (in the case where the cropped output is being compared to some larger volume).

The user has the option to save the cropped volume as a new **Data Container** or overwrite the current volume.

Normally this **Filter** will leave the origin of the volume set at (0, 0, 0), which means output files like the Xdmf file will have the same (0, 0, 0) origin. When viewing both the original larger volume and the new cropped volume simultaneously the cropped volume and the original volume will have the same origin which makes the cropped volume look like it was shifted in space. In order to keep the cropped volume at the same absolute position in space the user should turn **ON** the _Update Origin_ check box.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| X Min (Column) | int32_t | Lower X (column) bound of the volume to crop out |
| Y Min (Row)| int32_t | Lower Y (row) bound of the volume to crop out |
| Z Min (Plane)| int32_t | Lower Z (plane) bound of the volume to crop out |
| X Max (Column)| int32_t | Upper X (column) bound of the volume to crop out |
| Y Max (Row)| int32_t | Upper Y (row) bound of the volume to crop out |
| Z Max (Plane)| int32_t | Upper Z (plane) bound of the volume to crop out |
| Renumber Features | bool | Whether the **Features** should be renumbered |
| Update Origin | bool | Whether the origin of the cropped volume should be updated to the absolute location in the original volumes reference frame (*true*) or whether the origin of the original volume should be used as the origin of the cropped volume (*false*) |
| Save as New Data Container | bool | Specifies if the new grid of **Cells** should replace the current **Geometry** or if a new **Data Container** should be created to hold it |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | CellData | Cell | N/A | **Cell Attribute Matrix** that holds data for resolution change |
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs. Only required if _Renumber Features_ is checked |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | **Feature Attribute Matrix** that corresponds to the **Feature** data for the selected _Feature Ids_. Only required if _Renumber Features_ is checked |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | NewImageDataContainer | N/A | N/A | Created **Data Container** name with an **Image Geometry**. Only created if _Save as New Data Container_ is checked |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


