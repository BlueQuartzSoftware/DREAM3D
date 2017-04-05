Isolate Largest Feature (Identify Sample) {#identifysample}
=============

## Group (Subgroup) ##
Processing (Cleanup)

## Description ##
Often when performing a serial sectioning experiment (especially in the FIB-SEM), the sample is *overscanned* resulting in a border of *bad* data around the sample.  This **Filter** attempts to _identify_ the sample within the overscanned volume.  The **Filter** makes the assumption that there is only one contiguous set of **Cells** that belong to the sample. The **Filter** requires that the user has already *thresheld* the data to determine which **Cells** are *good* and which are *bad*.  The algorithm for the identification of the sample is then as follows:

1. Search for the largest contiguous set of *good* **Cells**. (This is assumed to be the sample)  
2. Change all other *good* **Cells**  to be *bad* **Cells**.  (This removes the "speckling" of what was *thresheld* as *good* data in the outer border region)

If _Fill Holes_ is set to *true*:
3. Search for the largest contiguous set of *bad* **Cells**. (This is assumed to be the outer border region)
4. Change all other *bad* **Cells**  to be *good* **Cells**.  (This removes the "speckling" of what was *thresheld* as *bad* data inside of the sample).

*Note:* if there are in fact "holes" in the sample, then this **Filter** will "close" them (if _Fill Holes_ is set to true) by calling all the **Cells** "inside" the sample *good*.  If the user wants to reidentify those holes, then reuse the threshold **Filter** with the criteria of *GoodVoxels = 1* and whatever original criteria identified the "holes", as this will limit applying those original criteria to within the sample and not the outer border region.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Fill Holes in Largest Feature | bool | Whether to fill holes within sample after it is identified |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | Mask | bool | (1) | *Mask* array defining what is sample and what is not |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


