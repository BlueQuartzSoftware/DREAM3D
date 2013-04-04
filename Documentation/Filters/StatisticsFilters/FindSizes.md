Find Sizes {#}
======
<h1 class="pHeading1">Find Sizes Filter</h1>
<p class="pCellBody">
<a href="../StatisticsFilters/FindSizes.html#wp2">Description</a> | <a href="../StatisticsFilters/FindSizes.html#wp3">Options</a> | <a href="../StatisticsFilters/FindSizes.html#wp4">Required Arrays</a> | <a href="../StatisticsFilters/FindSizes.html#wp5">Created Arrays</a> | <a href="../StatisticsFilters/FindSizes.html#wp1">Authors</a> 

Statistics Filters


This filter calculates the sizes of all features/grains.  The filter simply iterates through all voxels/datapoints asking the for feature/grain that owns them.  
The count of voxels/datapoints for each feature/grain is then stored as NumCells and the Volume and EquivalentDiameter are also calculated (and stored) by knowing 
the volume of each voxel/datapoint.

## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Field | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the feature/grain. |  |
| Field | NumCells | Number (int) of voxels/datapoints that are owned by the feature/grain. | This value does not place any distinction between voxels/datapoints that may be of a different size. |
| Field | Volumes | Volume (float) in um^3 of the feature/grain. | This value may be "redundant" from the NumCells value if all voxels/datapoints are the same size and is calculable from the EquivalentDiameters value. |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



