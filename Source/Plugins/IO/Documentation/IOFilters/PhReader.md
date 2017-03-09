Read Ph File (Feature Ids) {#phreader}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##
This **Filter** reads a custom data file used by the Materials Science Department at Carnegie Mellon University. The information in the file defines an **Image Geometry** with a set of **Feature** Ids.  The file format is a simple ASCII text file with the following contents:

**Format**

    Line 1:
    {5 Spaces}{x dimension}{5 Spaces}{y dimension}{5 Spaces}{z dimension}
    Line 2:
    'DREAM3'              52.00  1.000  1.0       {Number of Features}
    Line 3: (Without the quotes)
    " 0.000 0.000 0.000          0        "
    {Each Feature Id on a single line}

## Example Input ##

    11     11     11
    'DREAM3'              52.00  1.000  1.0       6
    0.000 0.000 0.000          0        
    830
    830
    0
    977
    977
    830
    830
    …..

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | The input .ph file path |
| Origin | flaot (3x) | The location in space of the (0, 0, 0) coordinate |
| Resolution | Float (x3) | The resolution values (dx, dy, dz) |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | ImageDataContainer | N/A | N/A | Created **Data Container** name with an **Image Geometry** |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** name |
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


