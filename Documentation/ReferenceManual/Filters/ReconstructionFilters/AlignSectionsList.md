Align Sections (List) {#}
======
<h1 class="pHeading1">Align Sections (List) Filter</h1>
<p class="pCellBody">
<a href="../ReconstructionFilters/AlignSectionsList.html#wp2">Description</a> | <a href="../ReconstructionFilters/AlignSectionsList.html#wp3">Options</a> | <a href="../ReconstructionFilters/AlignSectionsList.html#wp4">Required Arrays</a> | <a href="../ReconstructionFilters/AlignSectionsList.html#wp5">Created Arrays</a> | <a href="../ReconstructionFilters/AlignSectionsList.html#wp1">Authors</a> 

Reconstruction Filters


This Filter accepts a _.txt_ file that has full voxel/datapoints shifts that have already been calculated in it.  
The format of the file is as follows: 

_slice number, xshift, yshift
slice number, xshift, yshift 
slice number, xshift, yshift
etc..._
*Note: some/many of the other alignment filters write out a _.txt_ file with their calculated shifts in it; so
the user could manually modify those values, and then use them in this filter if the shifts in the other filter were deemed unsatisfactory.

## Parameters ## 

| Name | Type |
|------|------|
| Input File | File Path |

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



