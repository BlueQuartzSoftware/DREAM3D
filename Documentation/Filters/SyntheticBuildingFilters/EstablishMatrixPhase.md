Establish Matrix Phase {#}
======
<h1 class="pHeading1">Establish Matrix Phase Filter</h1>
<p class="pCellBody">
<a href="../Synthetic_BuilderFilters/EstablishMatrixPhase.html#wp2">Description</a> | <a href="../Synthetic_BuilderFilters/EstablishMatrixPhase.html#wp3">Options</a> | <a href="../Synthetic_BuilderFilters/EstablishMatrixPhase.html#wp4">Required Arrays</a> | <a href="../Synthetic_BuilderFilters/EstablishMatrixPhase.html#wp5">Created Arrays</a> | <a href="../Synthetic_BuilderFilters/EstablishMatrixPhase.html#wp1">Authors</a> 

Synthetic Builder Filters


To run this Filter properly, the _Initialize Synthetic Volume_ Filter must have been run already.
Based on the phase fraction in the statistics file used in the _Initialize Synthetic Volume_ Filter, this Filter assigns _voxels_ to a matrix phase. 

## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) | Values should be obtained from the input statistics file read in by IntializeSyntheticVolume |

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Field | Active | Boolean value specifying if the feature/grain is still in the sample (1 if the feature/grain is in the sample and 0 if it is not) | At the end of the filter, all features/grains will be "Active" as the "Inactive" features/grains will have been removed.  |
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



