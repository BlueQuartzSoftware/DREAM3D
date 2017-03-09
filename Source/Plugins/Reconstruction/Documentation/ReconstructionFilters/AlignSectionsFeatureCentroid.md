Align Sections (Feature Centroid) {#alignsectionsfeaturecentroid}
======

## Group (Subgroup) ##
Reconstruction (Alignment)

## Description ##
This **Filter** attempts to align 'sections' of the sample perpendicular to the Z-direction by determining the position that closest aligns the centroid(s) of previously defined "regions".  The "regions" are defined by a boolean array where the **Cells** have been flagged by another **Filter**. Typically, during reading/processing of the data, each **Cell** is subject to a "quality metric" (or threshold) that defines if the **Cell** is *good*.  This threshold can be used to define areas of each slice that are bad, either due to actual features in the microstructure or external references inserted by the user/experimentalist.  If these "regions" of *bad* **Cells** are believed to be consistent through sections, then this **Filter** will preserve that by aligning those "regions" on top of one another on consecutive sections. The algorithm of this **Filter** is as follows:

1. Determine the centroid of all **Cells** that are flagged with a boolean value equal to *true* for each section 
2. Determine the shifts that place centroids of consecutive sections on top of one another
3. Round the shifts determined in step 2 to the nearest multiple of the **Cell** resolution. (This forces the sections to be shifted by full **Cell** increments)

If the user elects to *Use Reference Slice*, then each section's centroid is shifted to lie on top of the reference slice's centroid position and not its neighboring section's centroid position.

**Note that this is algorithm cannot get caught in a local minimum!**

The user can choose to write the determined shift to an output file by enabling *Write Alignment Shifts File* and providing a file path.  

The user can also decide to remove a _background shift_ present in the sample. The process for this is to fit a line to the X and Y shifts along the Z-direction of the sample.  The individual shifts are then modified to make the slope of the fit line be 0.  Effectively, this process is trying to keep the top and bottom section of the sample fixed.  Some combinations of sample geometry and internal features can result in this algorithm introducing a 'shear' in the sample and the *Linear Background Subtraction* will attempt to correct for this.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Write Alignment Shift File | bool | Whether to write the shifts applied to each section to a file |
| Alignment File | File Path | The output file path where the user would like the shifts applied to the section to be written. Only needed if *Write Alignment Shifts File* is checked |
| Linear Background Subtraction | bool | Whether to remove a _background shift_ present in the alignment |
| Use Refernece Slice | bool | Whether the centroids of each section should be compared to a reference slice instead of their neighboring section |
| Reference Slice | int32_t | Slice number to use as reference. Only needed if *Use Reference Slice* is checked |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | Mask | bool | (1) | Specifies if the **Cell** is to be counted as part of the *sample* or not |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


