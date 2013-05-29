Bad Data Orientation Correlation {#neighborcicorrelation}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter first identifies all **Cells** that have a *confidence index* below the minimum set by the user.  Then, for each of those **Cells**, their neighboring **Cells** are checked to identify the neighbor **Cell** with the highest *confidence index*.  If the neighbor with the highest *confidence index* is above the minimum set by the user, then the data from that **Cell** will be copied to the **Cell** with the *confidence index* below the minimum.  If none of the neighbor **Cells** are above the minimum value, then no data will be copied to the **Cell** on that iteration.  
The filter will complete only one iteration by default, but can be set to *Loop Until Gone* (which will result in all **Cells** below the user defined minimum being replaced by neighboring **Cells** above the minimum). 


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
| Cell | Confidence Index | Scalar value defining the confidence in the orientation of the **Cell** (TSL data) |  | ReadH5ebsd |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



