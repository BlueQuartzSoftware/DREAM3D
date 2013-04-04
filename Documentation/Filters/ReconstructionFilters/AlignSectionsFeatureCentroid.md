
Align Sections (Feature Centroid) {#}
======
<h1 class="pHeading1">Align Sections (Feature Centroid) Filter</h1>
<p class="pCellBody">

<a href="../ReconstructionFilters/AlignSectionsFeatureCentroid.html#wp2">Description</a>
| <a href="../ReconstructionFilters/AlignSectionsFeatureCentroid.html#wp3">Options</a>
| <a href="../ReconstructionFilters/AlignSectionsFeatureCentroid.html#wp4">Required Arrays</a>
| <a href="../ReconstructionFilters/AlignSectionsFeatureCentroid.html#wp5">Created Arrays</a>
| <a href="../ReconstructionFilters/AlignSectionsFeatureCentroid.html#wp1">Authors</a> 

Reconstruction

This filter attempts to align consecutive serial sections by determining the position that closest aligns the centroid(s) of previously
defined "regions".  The "regions" are defined by a binary/boolean array where the voxels/datapoints have been flagged by another filter.
Typically, during reading of the data, each voxel/datapoint is subject to a "Quality Metric" (or threshold) that defines if the voxel/datapoint
is "good".  This threshold can be used to define areas of each slice that are bad, either due to actual features in the microstructure or
external references inserted by the user/experimentalist.  If these "regions" of "bad" voxels/datapoints are believed to be consistent through
sections, then this filter will preserve that by aligning those "regions" on top of one another on consecutive sections.
The algorithm of this filter is as follows:

1) Determine the centroid of all voxels/datapoints that are flagged with a boolean value of one (or true) for each section 
2) Determine the full voxel/datapoint shifts that place centroids of consecutive sections as close as possible.
*Note that this is algorithm cannot get caught in a local minima*


## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  |

## Created Arrays ##
None


## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



