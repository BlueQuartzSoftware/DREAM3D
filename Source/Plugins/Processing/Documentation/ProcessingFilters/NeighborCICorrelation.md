Neighbor CI Correlation {#neighborcicorrelation}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter first identifies all **Cells** that have a *confidence index* below the minimum set by the user.  Then, for each of those **Cells**, their neighboring **Cells** are checked to determine the neighbor **Cell** with maximum *confidence index*.  The attributes of the neighbor with the maximum *confidence index* are then reassigned to the reference **Cell**.

*Note:* By default the filter will run only one iteration of the cleanup.  If the user selects the *Loop Until Gone* option, then the filter will run iteratively until no **Cells** exist with a *confidence index* below the user defined value. So, if a **Cell** is below the tolerance and so are all of its neighbors, then that **Cell** will not be changed during that iteration and will remain unchanged until one of its neighbors gets changed by a **Cell** further away.  

## Parameters ##

| Name | Type |
|------|------|
| Minimum Confidence Index | Float |
| Loop Until Gone | Boolean |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | Confidence Index | Scalar value defining the confidence in the orientation of the **Cell** (TSL data) |


## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

