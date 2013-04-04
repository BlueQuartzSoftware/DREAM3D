Initialize Synthetic Volume {#}
======
<h1 class="pHeading1">Initialize Synthetic Volume Filter</h1>
<p class="pCellBody">
<a href="../Synthetic_BuilderFilters/InitializeSyntheticVolume.html#wp2">Description</a> | <a href="../Synthetic_BuilderFilters/InitializeSyntheticVolume.html#wp3">Options</a> | <a href="../Synthetic_BuilderFilters/InitializeSyntheticVolume.html#wp4">Required Arrays</a> | <a href="../Synthetic_BuilderFilters/InitializeSyntheticVolume.html#wp5">Created Arrays</a> | <a href="../Synthetic_BuilderFilters/InitializeSyntheticVolume.html#wp1">Authors</a> 

Synthetic Builder Filters


Based on __ShapeTypes__ in the __Ensemble__ this Filter utilizes the data in the
Statistics File (see StatsGenerator.app for information about creating a statistics file) designated by the user together with _voxel_ ranges and _resolution_
 desired for (x, y, z) to initialize the synthetic volume.

The (x, y, z) determine the size of the volume to be generated. The minimum voxel size is 1;
the maximum _voxel_ size can be large, but careful consideration is strongly suggested to
avoid memory problems.

The _resolution_ refers to the length of an edge of a _voxel_, usually microns, but
other measures of length can be used. __The minimum resolution for each dimension cannot
be 0.0, i.e, zero for any of the resolution values is forbidden as an input __.

The Filter produces an estimate of the number of grains in the volume associated with the
values the user entered.

<div align="center">
<img src="InitSynthVol.jpg" alt="InitSynthVol"/>
</div>



## Parameters ##

| Name | Type |
|------|------|
| Statistics File | Input File |
| X Voxels | Integer |
| Y Voxels | Integer |
| Z Voxels | Integer |
| X Res | Double |
| Y Res | Double |
| Z Res | Double |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | ShapeTypes | Enumeration (int) specifying the shape class of the features/grains of each Ensemble/phase (Ellipsoid=0, SuperEllipsoid=1, CubeOctahedron=2, Cylinder=3) | Values are obtained from user input on GUI |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** |  |
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



