Bad Data Orientation Correlation {#baddataneighbororientationcheck}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter compares the orientations of "*bad*" **Cells** with their neighbor **Cells**.  If the misorientation is below a user defined tolerance for a user defined number of neighbors, then the "*bad*" **Cell** will be changed to a "*good*" **Cell**.

## Parameters ##

| Name | Type |
|------|------|
| Misorientation Tolerance (Degrees) | Float |
| Required Number of Neighbors | Integer |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. | Find Cell Quaternions (Generic) |
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  | Single Threshold (Cell Data) (Processing), Multi Threshold (Cell Data) (Processing) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



