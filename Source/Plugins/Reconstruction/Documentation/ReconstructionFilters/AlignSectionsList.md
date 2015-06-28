Align Sections (List) {#alignsectionslist}
======

## Group (Subgroup) ##
Reconstruction (Alignment)

## Description ##
This **Filter** accepts a .txt file that has full **Cells** shifts that have already been calculated in it.  

The format of the file is as follows: 

    slice_number xshift yshift
    slice_number xshift yshift 
    slice_number xshift yshift
    etc...

The slices must be ordered from the bottom of the sample to the top of the sample (i.e., the first line of shifts should be the second slice relative to the first, then the next line is the third slice relative to the second, and so on). Also, the file must be either space or tab delimited.

*Note:* Some of the other alignment **Filters** have the option to write out a .txt file with their calculated shifts in it. The user could manually modify those values, and then use them in this **Filter** if the shifts in the other **Filter** were deemed unsatisfactory.  The format of the output file from other **Filters** is slightly different that the input format of this **Filter**, so some editing would be needed.

**This Filter will apply its shifts to the Data Container named ImageDataContainer in the current structure. If no Data Container with this name is present in the current structure, the Filter will not be able to function!**

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | The input .txt file path containing the shifts to apply to the sections |

## Required Geometry ##
Image

## Required Objects ##
None

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


