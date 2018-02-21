Segment Features (Scalar) 
======

## Group (Subgroup) ##

Reconstruction (Segmentation)

## Description ##

This **Filter** segments the **Features** by grouping neighboring **Cells** that satisfy the *scalar tolerance*, i.e., have a scalar difference less than the value set by the user. The process by which the **Features** are identified is given below and is a standard *burn algorithm*.

1. Randomly select a **Cell**, add it to an empty list and set its *FeatureId* to the current **Feature**
2. Compare the **Cell** to each of its six (6) face-sharing neighbors (i.e., calculate the scalar difference with each neighbor)
3. Add each neighbor **Cell** that has a scalar difference below the user defined tolerance to the list created in 1) and set the *FeatureId* of the neighbor **Cell** to the current **Feature**
4. Remove the current **Cell** from the list and move to the next **Cell** and repeat 2. and 3.; continue until no **Cells** are left in the list
5. Increment the current **Feature** counter and repeat steps 1. through 4.; continue until no **Cells** remain unassigned in the dataset

The user has the option to *Use Mask Array*, which allows the user to set a boolean array for the **Cells** that remove **Cells** with a value of *false* from consideration in the above algorithm. This option is useful if the user has an array that either specifies the domain of the "sample" in the "image" or specifies if the orientation on the **Cell** is trusted/correct. 

After all the **Features** have been identified, an **Attribute Matrix** is created for the **Features** and each **Feature** is flagged as *Active* in a boolean array in the matrix.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Scalar Tolerance | float | Tolerance  used to determine if neighboring **Cells** belong to the same **Feature** |
| Use Mask Array | bool | Specifies whether to use a boolean array to exclude some **Cells** from the **Feature** identification process |

## Required Geometry ##

Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | None | Any | (1) | Scalar array used during segmentation |
| **Cell Attribute Array** | Mask | bool | (1) | Specifies if the **Cell** is to be counted in the algorithm. Only required if *Use Mask Array* is checked |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs. |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | Created **Feature Attribute Matrix** name |
| **Feature Attribute Array** | Active | bool | (1) | Specifies if the **Feature** is still in the sample (*true* if the **Feature** is in the sample and *false* if it is not). At the end of the **Filter**, all **Features** will be _Active_ |


## Example Pipelines ##

+ (09) Image Segmentation

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


