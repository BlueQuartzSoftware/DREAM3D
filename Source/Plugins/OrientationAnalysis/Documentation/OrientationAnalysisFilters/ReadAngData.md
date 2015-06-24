Read EDAX EBSD Data (.ang) {#readangdata}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##
This Filter will read a single .ang file into a new **Data Container** with a corresponding **Image Geometry**, allowing the immediate use of **Filters** on the data instead of having to generate the intermediate .h5ebsd file. A **Cell Attribute Matrix** and **Ensemble Attribute Matrix** will also be created to hold the imported EBSD information. Currently, the user has no control over the names of the created **Attribute Arrays**. The user should be aware that simply reading the file then performing operations that are dependent on the proper crystallographic and sample reference frame will be undefined or simply **wrong**. In order to bring the [crystal reference frame](@ref rotateeulerrefframe) and [sample reference frame](@ref rotatesamplerefframe) into coincidence, rotations will need to be applied to the data.

### Default TSL Transformations ###
If the data has come from a TSL acquisition system and the settings of the acquisition software were in the default modes, he following reference frame transformations may need to be performed based on the version of the OIM Analysis software being used to collect the data:

+ Sample Reference Frame: 180<sup>o</sup> about the <010> Axis
+ Crystal Reference Frame: 90<sup>o</sup> about the <001> Axis

The user also may want to assign un-indexed pixels to be ignored by flagging them as "bad". The [Threshold Objects](@ref multithresholdobjects) **Filter** can be used to define this _mask_ by thresholding on values such as _Confidence Index_ > 0.1 or _Image Quality_ > desired quality.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path |The input .ang file path |
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
| Cell  | Confidence Index | Float |(1)     | Confidence of indexing  |
| Cell  | EulerAngles      | Float |(3)     | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z)  |
| Cell  | Fit              | Float |(1)     |  Quality of fit for indexing  |
| Cell  | Image Quality    | Float |(1) | Quality of image   |
| Cell  | Phases           | Int   | (1) | Specifies to which phase each **Cell** belongs   |
| Cell  | SEM Signal       | Float |(1) | Value of SEM signal   |
| Cell  | X Position       | Float |(1) | X coordinate of **Cell**   |
| Cell  | Y Position       | Float |(1) | Y coordinate of **Cell**   |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |
| Ensemble | LatticeConstants | Float | (6) | The 6 values that define the lattice constants for the phase |
| Ensemble | MaterialName | String | (1) | Name of each phase |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


