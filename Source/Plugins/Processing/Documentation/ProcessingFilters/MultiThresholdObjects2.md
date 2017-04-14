Threshold Objects 2 {#multithresholdobjects2}
=============

## Group (Subgroup) ##
Processing (Threshold)

## Description ##
This **Filter** allows the user to input single or multiple criteria for thresholding **Attribute Arrays** in an **Attribute Matrix**. Internally, the algorithm creates the output boolean arrays for each comparison that the user creates. Then all the output arrays are compared and if __any__ of the values for a specific **Object** is __false__, the corresponding **Object** in the final output array is marked as *false*. This is considered a logical "or" operation. An example of this **Filter's** use would be after EBSD data is read into DREAM.3D and the user wants to have DREAM.3D consider **Cells** that the user considers *good*. The user would insert this **Filter** and select the criteria that makes a **Cell** *good*. All arrays **must** come from the same **Attribute Matrix** in order for the **Filter** to execute. For example, an integer array contains the values 1, 2, 3, 4, 5. For a comparison value of 3 and the comparison operator greater than, the boolean threshold array produced will contain *false*, *false*, *false*, *true*, *true*.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Data Arrays to Threshold | Comparison List | This is the set of criteria applied to the objects the selected arrays correspond to when doing the thresholding |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any **Attribute Array** | None | Any | (1) | Array(s) selected in criteria set |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any **Attribute Array** | Mask | bool | (1) | Specifies whether the objects passed the set of criteria applied during thresholding |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


