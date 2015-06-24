Read Oxford Instr. Ebsd Data {#readctfdata}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##
This Filter will read a single .ctf file into a new **Data Container** with a corresponding **Image Geometry**, allowing the immediate use of **Filters** on the data instead of having to generate the intermediate .h5ebsd file. A **Cell Attribute Matrix** and **Ensemble Attribute Matrix** will also be created to hold the imported EBSD information. Currently, the user has no control over the names of the created **Attribute Arrays**. The user should be aware that simply reading the file then performing operations that are dependent on the proper crystallographic and sample reference frame will be undefined or simply **wrong**. In order to bring the [crystal reference frame](@ref rotateeulerrefframe) and [sample reference frame](@ref rotatesamplerefframe) into coincidence, rotations will need to be applied to the data.

### Default HKL Transformations ###
If the data has come from a HKL acquisition system and the settings of the acquisition software were in the default modes, then the following reference frame transformations need to be performed:

+ Sample Reference Frame: 180<sup>o</sup> about the <010> Axis
+ Crystal Reference Frame: None

The user also may want to assign un-indexed pixels to be ignored by flagging them as "bad". The [Threshold Objects](@ref multithresholdobjects) **Filter** can be used to define this _mask_ by thresholding on values such as _Error_ = 0.

### Radians and Degrees ###
2D .ctf files have their angles in **degrees**. Please be sure to [convert](@ref changeanglerepresentation) the Euler angles from degrees to radians before running any other **Filter**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path |The input .ctf file path |
| Data Container Name | String | Created **Data Container** name |
| Cell Attribute Matrix Name | String | Created **Cell Attribute Matrix** name |
| Cell Ensemble Attribute Matrix Name | String | Created **Ensemble Attribute Matrix** name |

## Required Geometry ##
Not Applicable

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | BC           | Float | (1) | Band contrast levels |
| Cell | EulerAngles  | Float | (3) | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z)  |
| Cell | BS           | Float | (1) | Band saturation levels |
| Cell | Bands        | Float | (1) | Number of bands   |
| Cell | Phases       | Int   | (1) | Specifies to which phase each **Cell** belongs   |
| Cell | MAD          | Float | (1) | Mean angular deviation. Typical threshold value is < 1.8  |
| Cell  | X Position       | Float |(1) | X coordinate of **Cell**   |
| Cell  | Y Position       | Float |(1) | Y coordinate of **Cell**   |
| Cell | Error        | Int   | (1) | Value = 0 is a well indexed scan point   |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |
| Ensemble | LatticeConstants | Float | (6) | The 6 values that define the lattice constants for the phase |
| Ensemble | MaterialName | String | (1) | Name of each phase |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

