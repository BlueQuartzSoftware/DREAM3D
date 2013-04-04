Align Sections (Misorientation) {#}
======
<h1 class="pHeading1">Align Sections (Misorientation) Filter</h1>
<p class="pCellBody">
<a href="../ReconstructionFilters/AlignSectionsMisorientation.html#wp2">Description</a> | <a href="../ReconstructionFilters/AlignSectionsMisorientation.html#wp3">Options</a> | <a href="../ReconstructionFilters/AlignSectionsMisorientation.html#wp4">Required Arrays</a> | <a href="../ReconstructionFilters/AlignSectionsMisorientation.html#wp5">Created Arrays</a> | <a href="../ReconstructionFilters/AlignSectionsMisorientation.html#wp1">Authors</a> 

Reconstruction Filters


This filter attempts to align consecutive serial sections by determining the position that results in the minimum amount of misorientation
between voxels/datapoints directly above-below each other. 
The algorithm of this filter is as follows:

1) Calculate the misorientation between each voxel/datapoint in a section and the voxel/datapoint directly above it in the next section.  
2) Count the number of voxel/datapoint pairs that have a misorientation above the user defined tolerance and store that as the misalignment value for that position.
3) Repeat steps 1 and 2 for each position when shifting the second slice (relative to the first) from three (3) voxels/datapoints to the left 
to three (3) voxels/datapoints to the right, as well as from three (3) voxels/datapoints up to three (3) voxels/datapoints down.
*Note that this creates a 7x7 grid*
4) Determine the position in the 7x7 grid that has the lowest misalignment value. (It will be the position with the fewest different voxel/datapoint pairs).
5) Repeat steps 1-4 with the center of each (new) 7x7 grid at the best position from the last 7x7 grid until the best position in the current/new 7x7 grid is the same
as the last 7x7 grid.
*Note that this is similar to a downhill simplex and can get caught in a local minima*

  


## Parameters ## 

| Name | Type |
|------|------| Comment |
| Misorientation Tolerance | Double | The value selected should be similar to the tolerance one would use to define grains (ie 2-10 degrees) |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GoodVoxels | Boolean values specifying whether a voxel/datapoint is of a quality acceptable to be used in the alignment process | Values are typically assigned when reading experimental data and applying a "Quality Metric" to define "good" and "bad" data  |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the voxel/datapoint in quaternion representation | Filter will calculate the quaternion for each voxel/datapoint if it is not already calculated. |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



