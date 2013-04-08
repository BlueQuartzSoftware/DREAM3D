Align Sections (List) {#alignsectionslist}
======

## Group (Subgroup) ##
Reconstruction Filters (Alignment)

## Description ##
This Filter accepts a _.txt_ file that has full **Cells** shifts that have already been calculated in it.  
The format of the file is as follows: 

    slice number, xshift, yshift
    slice number, xshift, yshift 
    slice number, xshift, yshift
    etc...
**Note**: some/many of the other alignment filters write out a **txt** file with their calculated shifts in it; so
the user could manually modify those values, and then use them in this filter if the shifts in the other filter were deemed unsatisfactory.

## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



