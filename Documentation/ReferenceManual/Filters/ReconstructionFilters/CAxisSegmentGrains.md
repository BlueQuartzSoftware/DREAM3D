Segment Grains (C-Axis Misorientation) {#}
======
<h1 class="pHeading1">Segment Grains (C-Axis Misorientation) Filter</h1>
<p class="pCellBody">
<a href="../ReconstructionFilters/CAxisSegmentGrains.html#wp2">Description</a> | <a href="../ReconstructionFilters/CAxisSegmentGrains.html#wp3">Options</a> | <a href="../ReconstructionFilters/CAxisSegmentGrains.html#wp4">Required Arrays</a> | <a href="../ReconstructionFilters/CAxisSegmentGrains.html#wp5">Created Arrays</a> | <a href="../ReconstructionFilters/CAxisSegmentGrains.html#wp1">Authors</a> 

Reconstruction Filters


<!-- Write all your documentation here --> 
The _c-axis misorientation_ refers to the difference in rotation angle, about the c-axis in the hexagonal system, that is present between __Cell__s. This Filter searches for and identifies those __Cell__s that satisfy the tolerance angle (1-5 degrees) entered by the user. 

## Parameters ## 

| Name | Type |
|------|------|
| C-Axis Misorientation Tolerance | Double |

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the voxel/datapoint in quaternion representation | Filter will calculate the quaternion for each voxel/datapoint if it is not already calculated. |

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. |  |
| Field | Active | Boolean value specifying if the feature/grain is still in the sample (1 if the feature/grain is in the sample and 0 if it is not) | At the end of the filter, all features/grains will be "Active" as the "Inactive" features/grains will have been removed.  |
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



