# Read Feature Info File #


## Group (Subgroup) ##

IO (Input)

## Description ##

This **Filter** reads information about an existing set of **Feature** into the a new **Feature Attribute Matrix**. 
The basic format of the file is the number of **Features** on the first line of the file followed by columnar 
formatted data listing the:

+ **Feature** Id
+ **Ensemble** Id
+ Euler Angle 1
+ Euler Angle 2
+ Euler Angle 3

Currently, only the **Ensemble** Id and Euler angle information can be 
read using this **Filter**.

Values **MUST** be seperated by a _SPACE_ character.

Comment lines in the file **MUST** have '#' as the first character of the line. Comments
can be anywhere in the file.

If the Euler values are in Degrees be sure to use the "Degrees to Radians" filter
to convert those values to Radians.


*If you do not have Euler Angle data for each **Feature** then substituting Zero (0.0) is fine.*

**Feature** and **Ensemble** Ids start from one (1) in DREAM.3D. DREAM.3D uses the 
zero (0) index internally which has several ramifications that the user needs to 
understand. Statistics will **not** be calculated for any **Feature** or **Ensemble** 
with an Id value of zero (0). If your segmented data numbering starts with Zero (0), 
then the user needs to find a way to renumber **Feature**=0 to another value, 
typically the max Id + 1. For example, if you have a file with 123 **Features** 
and 2 **Ensembles** then the file would be similar to the following: 

## Example Input ##

For example, if you have a file with 8 **Features** and 2 **Ensembles** then the file would be similar to the following:

    # Number of Features
    8
    # Grain ID Ensemble ID Euler 1 Euler 2 Euler 3
    1 1 0. 0. 0.
    2 2 0.3490658504 0. 0.
    3 1 0.523598776 0. 0.
    4 2 0.69813 0. 0.
    5 1 0. 0.785398 0.
    6 2 0.3490658504 0.785398 0.
    7 1 0.523598776 0.785398 0.
    8 1 0.69813 0.785398 0.
    
Note the use of the comments to add more verbosity to the file. The comment lines are optional.


## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Input Feature Info File | File Path | The input file path |
| Create Element Level Arrays | bool | Whether to create **Element** level arrays for the imported data |
| Renumber Features | bool | Whether to randomly shuffle the Feature Ids |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|----------|
| **Element Attribute Array** | EulerAngles | float | (3)  | Three angles defining the orientation of the **Element** in Bunge convention (Z-X-Z). Only created if _Create Cell Level Arrays_ is checked |
| **Element Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Element** belongs. Only created if _Create Cell Level Arrays_ is checked |
| **Feature Attribute Array** | EulerAngles | float | (3) | Three angles defining the orientation of each **Feature** in Bunge convention (Z-X-Z) |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |

## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


