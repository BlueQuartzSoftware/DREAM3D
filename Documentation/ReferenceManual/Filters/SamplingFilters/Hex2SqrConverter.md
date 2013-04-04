<html>
<head>
<meta name="qrichtext" content="1" />
<style type="text/css">
h1.pHeading1 { color: #003366; font-family: Arial, Verdana, Helvetica, sans-serif; font-size: x-large; font-weight: bold; text-align: left }
h2.pHeading2 { color: #003366; font-family: Arial, Verdana, Helvetica, sans-serif; font-size: large; font-weight: bold; text-align: left }
p.pBody { font-family: Arial, Verdana, Helvetica, sans-serif; font-size: medium; text-align: left }
p.pCellBody { font-family: Arial, Verdana, Helvetica, sans-serif; font-size: medium; text-align: left }
</style>Convert Hexagonal Grid Data to Square Grid Data {#}
======
<h1 class="pHeading1">Convert Hexagonal Grid Data to Square Grid Data Filter</h1>
<p class="pCellBody">
<a href="../SamplingFilters/Hex2SqrConverter.html#wp2">Description</a> | <a href="../SamplingFilters/Hex2SqrConverter.html#wp3">Options</a> | <a href="../SamplingFilters/Hex2SqrConverter.html#wp4">Required Arrays</a> | <a href="../SamplingFilters/Hex2SqrConverter.html#wp5">Created Arrays</a>| <a href="../SamplingFilters/Hex2SqrConverter.html#wp1">Authors</a> 

Sampling Filters

This filter will convert Hexagonal Gridded TSL .ang files into Square Grids by means of 
interpolation between points. Note that depending on how this filter is used the new
files may have more or less rows of data and that some data may be interpolated based on
its neighbor data.

Run this filter by itself in order to convert all of your Hex Grid .ang files into square
grid .ang files. Then use the "Import Ebsd Data to H5Ebsd file" filter to import your data
into a .h5ebsd file archive.


## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



