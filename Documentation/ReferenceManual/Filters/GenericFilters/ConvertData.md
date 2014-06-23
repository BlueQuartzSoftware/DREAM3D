Convert Data (Voxel Cell) {#convertdata}
=====

## Group (Subgroup) ##
Generic (Misc)



## Description ##

This filter converts data from one primitive type to another by using the built in translation of the compiler. This filter can be used if the user needs to convert an array into a type that is accepted by another filter. For example a filter may need an input array to be of type _int32_t_ but the array that the user would like to use is _uint16_t_. The user may use this filter to create a new array that has the proper target type (_int32_t_).

**This filter is here for convenience and should be used with great care and understanding of the input and output data. This filter should rarely be required and if the user thinks that they require this filter then a detailed examination of all the data involved should be undertaken to avoid possible undefined behaviors.**

### IMPORTANT NOTES ###

**Up Casting**

Upcasting is most likely well defined by the compilers. This is the act of creating a new array using a primitive value that is represented by more bytes than the original data. For example if the user converts 1 byte integers into 4 byte integers or converted 4 byte floats into 8 byte floats.

**Down Casting**

Down casting can have undefined behavior depending on the primitive types involved. Down casting is the opposite of up casting and involves converting data from a larger byte count representation to a representation of lower byte count. For example converting 4 byte integers into 2 byte integers or 8 byte floats into 4 byte floats. What happens to the data all depends on the range of values in the original array. If the target type's range can hold all the values of the original arrays values then the conversion would have a well defined outcome.

**Signed/Unsigned Conversions**

When converting data from signed values to unsigned values or vice-versa then there can also be undefined behavior. For example if the user were to convert a signed 4 byte integer array to an unsigned 4 byte integer array and the input array has negative values then the conversion rules are undefined and may differ from operating system to operating system.

## Parameters ##
| Name             | Type |
|------------------|------|
| Input Array Name | String |  
| Scalar Type      | Enumeration |  
| Output Array Name | String |

## Required Arrays ##

The array that is being converted must be available during runtime of the filter

## Required Data Containers ##

Volume DataContainer


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Variable  | Depends on User Selection | The converted array | other   |



## Authors ##

**Copyright** 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

