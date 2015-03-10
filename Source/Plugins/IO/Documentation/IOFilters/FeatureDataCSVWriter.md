Write Feature Data as CSV File {#featuredatacsvwriter}
======

## Group (Subgroup) ##
I/O Filters (Output)


## Description ##
This Filter writes the **Feature** data, using a filename input by the user, to an output file in *CSV* format. Every array in the **Feature** map is written as a column of data in the csv file.  The user can choose to write the neighbor data. Neighbor data are data arrays that are associated with the neighbors of a **Feature**, such as: list of neighbors, list of misorientations, list of shared surface areas, etc. These blocks of info are written after the scalar data arrays.  Since the number of neighbors is variable for each **Feature**, the data is written as follows (for each **Feature**): Id, number of neighbors, value1, value2,...valueN.


## Parameters ##

| Name | Type |
|------|------|
| Output File | File Path |
| Write Neighbor Data | Boolean (On of Off) |

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

