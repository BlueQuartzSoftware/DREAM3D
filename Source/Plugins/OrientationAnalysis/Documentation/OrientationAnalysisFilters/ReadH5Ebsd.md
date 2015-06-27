Read H5EBSD File {#readh5ebsd}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##

This **Filter** reads from the .h5ebsd file that was generated with the [Import Orientation File(s) to H5EBSD](ebsdtoh5ebsd.html) **Filter**. The user can use the checkboxes under the _Data Arrays to Read_ section to select which specific data arrays they are interested in importing. The user can select a subset of the slices if they do not wish to import the entire volume. The type of transformations that are recommended based on the manufacturer of the data are also listed with a checkbox that allows the user to toggle the recommended transformations on and off.

-----

![Read H5Ebsd File User Interface](images/ReadH5Ebsd_UI.png)
@image latex images/ReadH5Ebsd_UI.png "Read H5Ebsd File User Interface" width=6in

-----

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | The input .h5ebsd file path |
| Start Slice | Int | The first slice of data to read |
| End Slice | Int | The last slice of data to read |
| Use Recommended Transformations | bool | Whether to apply the listed recommended transformations |
| Data Arrays to Read | Bool(s) | Whether to read the listed arrays |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container**  | ImageDataContainer | N/A | N/A    | Created **Data Container** name with an **Image Geometry** |
| **Attribute Matrix**  | CellData | Cell | N/A    | Created **Cell Attribute Matrix** name  |
| **Attribute Matrix**  | CellEnsembleData | Cell Ensemble | N/A    | Created **Cell Ensemble Attribute Matrix** name  |

An array for each of the arrays selected in the _Data Arrays to Read_ section will also be created.

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


