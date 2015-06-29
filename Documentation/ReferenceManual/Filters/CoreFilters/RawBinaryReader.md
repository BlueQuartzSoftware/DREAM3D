Raw Binary Reader {#rawbinaryreader}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##

This **Filter** is designed to read data stored in files on the users system in _binary_ form. The data file should **not** have any type of header before the data in the file. The user should know exactly how the data is stored in the file and properly define this in the user interface. Not correctly identifying the type of data can cause serious issues since this **Filter**  is simply reading the data into a pre-allocated array interpreted as the user defines.

This **Filter**  will error out and block the **Pipeline** from running if the total number of bytes that would need to be read from the file is larger than the actual file itself. The user can use an input file that is actually **larger** than the number of bytes required by the **Filter**; in this case, the **Filter**  will only read the first part of the file unless an amount of bytes to skip is set.

The amount of data that is in the file should match the **Attribute Matrix** dimensions that the data will be read into.

### Scalar Type ###

Computer data comes in 10 basic types on modern 32 bit and 64 bit operating systems. Data can be categorized as either _integer_ or _floating point_. With each of these types, the number of bits that represent the data determine their maximum and minimum values. For integer values, the standard types are 8, 16, 32 and 64 bit (1, 2, 4, and 8 bytes). For floating point values, there are either 32 bit or 64 bit (4 or 8 bytes). Integer types can be either _signed_ or _unsigned_. A signed integer can take negative values. An unsigned integer can only take positive values, but will have twice the positive value range as a signed integer.

The types of data that can be read with this **Filter** include:

    signed Int8
    unsigned UInt8
    signed Int16
    unsigned UInt16
    signed Int32
    unsigned UInt32
    signed Int64
    unsigned UInt64
    Float 32 bit
    Double 64 bit

---


### Number of Components ###

This parameter tells the program how many values are present for each _tuple_. For example, a grayscale image would typically have just a single value of type unsigned 8 bit integer at every pixel/voxel. A color image will have at least 3 components for red (R), breen (G) and blue (B), and sometimes 4 values if the alpha (A) channel is also stored. Euler angles are typically stored as a 3 component vector of 32 bit floating point values.

### Endian ###

This parameter tells the program which byte is _most significant_ for multibyte values. Intel architecture computers are little endian while Power PC, Sun Sparc and DEC Alpha CPUs are big endian. Consider the following example:

**Byte Ordering Example for 32 Bit Signed Integer**

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Interpretation |
|---|---|---|---|----------------|
| FF | AA | 00 | 00 | -5636096 (Big Endian) |
| 00 | 00 | AA | FF | 43775 (Little Endian) |

This setting is _crucial_ to the correct interpretation of the binary data, so the user must be aware of how their binary data was encoded.


### Skip Header Bytes ###

If the raw binary file you are reading has a _header_ before the actual data begins, the user can instruct the **Filter** to skip this header portion of the file. The user needs to know how lond the header is in bytes. Another way to use this value is if the user wants to read data out of the interior of a file by skipping a defined number of bytes.


## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | The input binary file path |
| Scalar Type | Enumeration | Data type of the binary data |
| Number of Components | int32_t | The number of values at each tuple |
| Endian | Enumeration | The endianness of the data |
| Skip Header Bytes | int32_t | Number of bytes to skip before reading data |

## Required Geometry ##

Not Applicable

## Required Objects ##

None

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any  | None     | Any  | Any                  |  Created **Attribute Array** name |



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


