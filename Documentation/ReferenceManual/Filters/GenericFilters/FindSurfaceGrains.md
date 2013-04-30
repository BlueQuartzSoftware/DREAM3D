Find Surface Fields {#findsurfacegrains}
==========

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter determines whether a **Field** touches an outer surface of the sample. This is accomplished by simply querying the **Field** owners of the **Cells** that sit at either xMin, xMax, yMin, yMax, zMin or zMax.
Any **Field** that owns one of those **Cells** is said to touch an outer surface and all other **Fields** are said to not touch an outer surface of the sample.


NOTE: If the structure/data is actually 2D, then the dimension that is planar is not considered and only the **Fields** touching the edges are considered surface **Fields**.


## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Fields (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | SurfaceFields | Boolean flag equal to 1 if the **Field** touches an outer surface of the sample and equal to 0 if it does not. |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



