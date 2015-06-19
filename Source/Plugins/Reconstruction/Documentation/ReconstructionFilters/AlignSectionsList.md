Align Sections (List) {#alignsectionslist}
======

## Group (Subgroup) ##
Reconstruction Filters (Alignment)

## Description ##
This Filter accepts a _.txt_ file that has full **Cells** shifts that have already been calculated in it.  
The format of the file is as follows: 

    slice_number xshift yshift
    slice_number xshift yshift 
    slice_number xshift yshift
    etc...
**Note**: the slices must be ordered from teh bottom of the sample to the top of the sample (i.e. the first line of shifts should be the second slice relative to the first, then the next line is the third slice relative to the second, and so on...)
**Note**: the file must be either space or tab delimited.

**Note**: some/many of the other alignment filters have the option to write out a **txt** file with their calculated shifts in it; so the user could manually modify those values, and then use them in this filter if the shifts in the other filter were deemed unsatisfactory.  The format of the output file from other filters is slightly different that the input format of this filter, so some editing would be needed.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | Path to the file containgint eh shifts to apply to the sections |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
None

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


