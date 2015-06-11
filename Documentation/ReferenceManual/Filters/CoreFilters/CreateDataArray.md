Create Data Array {#createdataarray}
=====

## Group (Subgroup) ##
Core Filters (Generation)


## Description ##
This filter creates a DataArray of any primitive type with any number of components (1 Row x N Cols) and initialize all the values to a user settable value. The primitive type of the array to create is set using an integer constant. Code listing 1 shows the values:

**Code Listing 1**

    static const int Int8 = 0;
    static const int UInt8 = 1;
    static const int Int16 = 2;
    static const int UInt16 = 3;
    static const int Int32 = 4;
    static const int UInt32 = 5;
    static const int Int64 = 6;
    static const int UInt64 = 7;
    static const int Float = 8;
    static const int Double = 9;
    static const int Bool = 10;
    

**Data Types** 

| Type             | Size |        Range       |
|------------------|------|--------------------|
| Signed Integer | 8 bit |0 to 255|
| Unsigned Integer | 8 bit |-128 to 127|
| Signed Integer | 16 bit |-32,768 to 32,767|
| Unsigned Integer | 16 bit |0 to 65,535|
| Signed Integer | 32 bit |-2,147,483,648 to 2,147,483,647|
| Unsigned Integer | 32 bit |0 to 4,294,967,295|
| Signed Integer | 64 bit |–9,223,372,036,854,775,808 to 9,223,372,036,854,775,807|
| Unsigned Integer | 64 bit |0 to 18,446,744,073,709,551,615|
| Float | 32 bit | -3.4e+38 to -1.1e-38, 0.0, 1.1e-38 to 3.4e+38 (7 digits)|
| Double | 64 bit | -1.7e+308 to -2.2e-308, 0.0, 2.2e-308 to 1.7e+308 (15 digits)|
| Boolean | 8 bit |0 = false and any other value will be forced to 1 = true|

The number of components should be at least 1. An example of "Number of Components" would be 3 for an RGB Image, 1 for a gray scale image, 1 for a simple array, 4 for a Quaternions array. All values of the array will be initialized to the user set value. The initialization value text box
must have a user entry or the default value will be used.

## Parameters ##

| Name             | Type | Description |
|------------------|------|-------------|
| Output Array Name | String | Name for created array |
| Scalar Type | Integer | Data type for created array |
| Number of Components | Integer | Greater than 1 |
| Initialization Value | double | Fill value for array |

## Required Geometry ##
Not Applicable

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|----------------|
| Any | None | User chosen | User chosen | Output Array Name |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

