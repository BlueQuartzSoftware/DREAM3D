Find Relative Motion Between Slices {#findrelativemotionbetweenslices}
=============

## Group (Subgroup) ##
Processing Filters (Image)

## Description ##
This Filter assigns a direction *moved* to each **Cell** by extracting a patch of *user defined* size, centered at each **Cell**, moving it up a *user defined* number of *slices* and translating it while looking for the minimum mean squared distance between the patch and the slice to which it was shifted.  The center of the patch when it has the minimum mean squared difference is said to be the point to which the **Cell** moved.  A vector is drawn from the **Cell** to the point where the **Cell** moved and that vector is normalized and stored as a unit vector on the **Cell**.
The Filter allows the user to chose which plane the patches are extracted from and moved perpendicular to when moving *slices*.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Plane of Interest | Selection | Tells the Filter which plane (XY, XZ, YZ) to project perpendicular to |
| Patch Size 1 (Voxels) | Int | Size of the patch centered on each pixel in the first dimension of the plane of interest |
| Patch Size 2 (Voxels) | Int | Size of the patch centered on each pixel in the second dimension of the plane of interest |
| Search Size 1 (Voxels) | Int | Size of the search window centered on each pixel in the first dimension of the plane of interest |
| Search Size 2 (Voxels) | Int | Size of the search window centered on each pixel in the first dimension of the plane of interest |
| Slice Step (Voxels) | Int | Number of slices to move up perpendicular to the plane of interest |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | None | Any, except Boolean | (1) | Any scalar array the user would like to use to track *motion* of **Features** through slices |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | MotionDirection | Float | (3) | Specifies the direction, as a unit vector, that the extracted patch *moved* |

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


