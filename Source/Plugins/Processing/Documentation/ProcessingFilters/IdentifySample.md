Identify Sample {#identifysample}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
Often when performing a serial-sectioning experiment (especially in the FIB-SEM), the sample is "overscanned" resulting in a border of "bad" data around the sample.  This Filter attempts to "identify" the sample within the overscanned volume.  The filter makes the assumption that there is only one contiguous set of **Cells** that belong to the sample.  The filter requires that the user has already *thresheld* the data to determine which **Cells** are "good" and which are "bad".  The algorithm for the identification of the sample is then as follows:

1. Search for the largest contiguous set of "good" **Cells**. (This is assumed to be the sample)  
2. Change all other "good" **Cells**  to be "bad" **Cells**.  (This removes the "speckling" of what was *thresheld* as "good" data in the outer border region).
3. Search for the largest contiguous set of "bad" **Cells**. (This is assumed to be the outer border region)
4. Change all other "bad" **Cells**  to be "good" **Cells**.  (This removes the "speckling" of what was *thresheld* as "bad" data inside of the sample).

Note: if there are in fact "holes" in the sample, then this filter will "close" them by calling all the **Cells** "inside" the sample "good".  If the user wants to reidentify those holes, then use the [MultiThreshold Cells Filter](multithresholdcells.html) and use criteria of *GoodVoxels = 1* and whatever other criteria would identify the "holes", as this will limit applying those criteria to within the sample and not the outer border region.

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels |  |  | Single Threshold (Cell Data) (Processing), Multi Threshold (Cell Data) (Processing) |


## Created Arrays ##
None

## Authors ##


**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

