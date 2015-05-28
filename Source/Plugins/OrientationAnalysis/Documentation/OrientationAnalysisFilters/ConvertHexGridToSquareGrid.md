
Convert Hexagonal Grid Data to Square Grid Data (TSL - .ang) {#converthexgridtosquaregrid}
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


## Required Arrays ##


## Created Arrays ##

## Authors ##






## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


