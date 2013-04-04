Find Surface Grains {#findsurfacegrains}
==========

## Group ##
Generic Filters

## Description ##
This filter determines whether a feature/grain touches an outer surface of the sample. This is accomplished by simply querying the feature/grain owners of the voxels/datapoints that sit at either xMin, xMax, yMin, yMax, zMin or zMax.
Any feature/grain that owns one of those voxels/datapoints is said to touch an outer surface and all other features/grains are said to not touch an outer surface of the sample.



## Parameters ##

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Field | SurfaceFields | Boolean flag equal to 1 if the feature/grain touches an outer surface of the sample and equal to 0 if it does not. |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



