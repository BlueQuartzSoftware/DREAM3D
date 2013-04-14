
Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang) {#hex2sqrconverter}
======

## Group (Subgroup) ##
Sampling Filters (Resolution)

## Description ##
This filter will convert Hexagonal Gridded TSL .ang files into Square Grids by means of 
interpolation between points. Note that depending on how this filter is used the new
files may have more or less rows of data and that some data may be interpolated based on
its neighbor data.  By default, the spacing of the square grid will be the spacing between columns in the heagonal grid.

Run this filter by itself in order to convert all of your Hex Grid .ang files into square
grid .ang files. Then use the "Import Ebsd Data to H5Ebsd file" filter to import your data
into a .h5ebsd file archive.


## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



