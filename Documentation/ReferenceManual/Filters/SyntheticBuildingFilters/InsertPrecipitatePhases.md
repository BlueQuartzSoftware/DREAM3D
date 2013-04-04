Insert Precipitate Phases {#}
======
<h1 class="pHeading1">Insert Precipitate Phases Filter</h1>
<p class="pCellBody">
<a href="../Synthetic_BuilderFilters/InsertPrecipitatePhases.html#wp2">Description</a> | <a href="../Synthetic_BuilderFilters/InsertPrecipitatePhases.html#wp3">Options</a> | <a href="../Synthetic_BuilderFilters/InsertPrecipitatePhases.html#wp4">Required Arrays</a> | <a href="../Synthetic_BuilderFilters/InsertPrecipitatePhases.html#wp5">Created Arrays</a> | <a href="../Synthetic_BuilderFilters/InsertPrecipitatePhases.html#wp1">Authors</a> 

Synthetic Builder Filters


If precipitate phases have been included in the _Statistics_ file used by the _Initialize Synthetic Volume_ Filter, _voxels_ are assigned based on the volume fraction of the precipitate phase.
 Periodic Boundary conditions are selected by the user to be active or inactive. <BR>


## Parameters ##

| Name | Type |
|------|------|
| Periodic Boundary | Boolean (On or Off) |

## Required Arrays ##



| Cell | CellPhases |  |
| Cell | GrainIds |  |
| Cell | SurfaceVoxels |  |
| Ensemble | PhaseTypes |  |
| Ensemble | ShapeTypes |  |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | Active |  |
| Field | AxisEulerAngles |  |
| Field | AxisLengths |  |
| Field | Centroids |  |
| Field | EquivalentDiameters |  |
| Field | FieldPhases |  |
| Field | Neighborhoods |  |
| Field | NumCells |  |
| Field | Omega3s |  |
| Field | Volumes |  |
| Ensemble | NumFields |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



