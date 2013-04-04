Raw Binary Reader {#rawbinaryreader}
======

## Group ##
IO Filters

## Description ##
 This filter is designed to read data stored in files on the users system that are stored in their
 binary form versus ascii form. The data file should __NOT__ have any type of header before the data
 in the file. The user should know exactly how the data is stored in the file and properly define this
 in the user interface. Not correctly identifying the type of data can cause serious issues because on
 a low level this filter is simply reading the data into a preallocated array and interpreted as the user
 defines. This filter can be used as a sort of bridge to read in binary data from sources like IDL and MatLab.


## Parameters ##

| Name | Type |
|------|------|
| Scalar Type | Choices |

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




