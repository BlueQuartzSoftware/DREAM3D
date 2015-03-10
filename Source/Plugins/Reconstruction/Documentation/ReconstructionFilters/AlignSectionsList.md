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
**Note**: the file must be either space or tab delimited.

**Note**: some/many of the other alignment filters write out a **txt** file with their calculated shifts in it; so
the user could manually modify those values, and then use them in this filter if the shifts in the other filter were deemed unsatisfactory.  The format of the output file from other filters is slightly different that the input format of this filter, so some editing would be needed.

## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |

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

