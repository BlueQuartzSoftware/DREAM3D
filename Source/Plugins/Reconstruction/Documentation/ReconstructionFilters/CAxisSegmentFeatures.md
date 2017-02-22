Segment Features (C-Axis Misalignment) {#caxissegmentfeatures}
======

## Group (Subgroup) ##
Reconstruction (Segmentation)

## Description ##
This **Filter** segments the **Features** by grouping neighboring **Cells** that satisfy the *C-axis misalignment tolerance*, i.e., have misalignment angle less than the value set by the user. The *C-axis misalignment* refers to the angle between the <001> directions (C-axis in the hexagonal system) that is present between neighboring **Cells**.  The process by which the **Features** are identified is given below and is a standard *burn algorithm*.

1. Randomly select a **Cell**, add it to an empty list and set its *Feature Id* to the current **Feature**
2. Compare the **Cell** to each of its six (6) face-sharing neighbors (i.e., calculate the c-axis misalignment with each neighbor)
3. Add each neighbor **Cell** that has a C-axis misalignment below the user defined tolerance to the list created in 1. and set the *Feature Id* of the neighbor **Cell** to the current **Feature**
4. Remove the current **Cell** from the list and move to the next **Cell** and repeat 2. and 3.; continue until no **Cells** are left in the list
5. Increment the current **Feature** counter and repeat steps 1. through 4.; continue until no **Cells** remain unassigned in the dataset

The user has the option to *Use Mask Array*, which allows the user to set a boolean array for the **Cells** that remove **Cells** with a value of *false* from consideration in the above algorithm. This option is useful if the user has an array that either specifies the domain of the "sample" in the "image" or specifies if the orientation on the **Cell** is trusted/correct. 

After all the **Features** have been identified, a **Feature Attribute Matrix** is created for the **Features** and each **Feature** is flagged as *Active* in a boolean array in the matrix.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| C-Axis Misorientation Tolerance (Degrees) | float | Tolerance (in degrees) used to determine if neighboring **Cells** belong to the same **Feature** |
| Use Mask Array | bool | Specifies whether to use a boolean array to exclude some **Cells** from the **Feature** identification process |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | Quats | float | (4) | Specifies the orientation of the **Cell** in quaternion representation |
| **Cell Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs |
| **Cell Attribute Array** | Mask | bool | (1) | Specifies if the **Cell** is to be counted in the algorithm. Only required if *Use Mask Array* is checked |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs. |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | Created **Feature Attribute Matrix** name |
| **Feature Attribute Array** | Active | bool | (1) | Specifies if the **Feature** is still in the sample (*true* if the **Feature** is in the sample and *false* if it is not). At the end of the **Filter**, all **Features** will be _Active_ |



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


