# Import Bruker Nano Esprit EBSD Data (.h5) #


## Group (Subgroup) ##

Import/Export (Import)

## Description ##

This **Filter** will read a single .h5 file into a new **Data Container** with a corresponding **Image Geometry**, allowing the immediate use of **Filters** on the data instead of having to generate the intermediate .h5ebsd file. A **Cell Attribute Matrix** and **Ensemble Attribute Matrix** will also be created to hold the imported EBSD information. Currently, the user has no control over the names of the created **Attribute Arrays**.

| User interface before entering a proper "Z Spacing" value and selecting which scans to include. |
|-------|
|![](Images/ReadEDAXH5_1.png)|


| User interface AFTER setting the "Z Spacing" and selecting files.  |
|-------|
|![](Images/ReadEDAXH5_2.png)|


## Notes About Reference Frames ##

The user should be aware that simply reading the file then performing operations that are dependent on the proper crystallographic and sample reference frame will be undefined or simply **wrong**. In order to bring the [crystal reference frame](@ref rotateeulerrefframe) and [sample reference frame](@ref rotatesamplerefframe) into coincidence, rotations will need to be applied to the data. The recommended filters are:

+ [Rotate Euler Reference Frame](rotateeulerrefframe.html)
+ [Rotate Sample Reference Frame](rotatesamplerefframe.html)

If the data has come from a TSL acquisition system and the settings of the acquisition software were in the default modes, the following reference frame transformations may need to be performed based on the version of the OIM Analysis software being used to collect the data:

+ Sample Reference Frame: 180<sup>o</sup> about the <010> Axis
+ Crystal Reference Frame: 90<sup>o</sup> about the <001> Axis

The user also may want to assign un-indexed pixels to be ignored by flagging them as "bad". The [Threshold Objects](@ref multithresholdobjects) **Filter** can be used to define this _mask_ by thresholding on values such as _MAD_ > xx.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | The input .h5 file path |
| Scan Name | Enumeration | The name of the scan in the .h5 file. EDAX can store multiple scans in a single file |
| Z Spacing | float | The spacing in microns between each layer. |
| Origin | float (3x1) | The origin of the volume |
| Read Pattern Data | bool | Default=OFF |

## Required Geometry ##

Not Applicable

## Required Objects ##

None

## Created Objects ##


| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container**  | ImageDataContainer | N/A | N/A    | Created **Data Container** name with an **Image Geometry** |
| **Attribute Matrix**  | CellData | Cell | N/A    | Created **Cell Attribute Matrix** name  |
| **Attribute Matrix**  | CellEnsembleData | Cell Ensemble | N/A    | Created **Cell Ensemble Attribute Matrix** name  |

### Created Cell Attribute Arrays ###

These arrays will **most likely** be created but is not guaranteed. Additional arrays (unknown at the time of writing) may also be created.

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array**  | DD | float | (1) |  |
| **Cell Attribute Array**  | MAD | float | (1) | Mean Angular Deviation |
| **Cell Attribute Array**  | MADPhase | int | (1) |  |
| **Cell Attribute Array**  | NIndexedBands | int | (1) |  |
| **Cell Attribute Array**  | PCX | float | (1) | Pattern Center X |
| **Cell Attribute Array**  | PCY | float | (1) | Pattern Center Y |
| **Cell Attribute Array**  | Phase | int | (1) |  |
| **Cell Attribute Array**  | RadonBandCount | int | (1) |  |
| **Cell Attribute Array**  | RadonQuality | float | (1) |  |
| **Cell Attribute Array**  | RawPatterns | float | (1) |  |
| **Cell Attribute Array**  | XBEAM | int | (1) |  |
| **Cell Attribute Array**  | YBEAM | int | (1) |  |
| **Cell Attribute Array**  | XSAMPLE | float | (1) |  |
| **Cell Attribute Array**  | YSAMPLE | float | (1) |  |
| **Cell Attribute Array**  | Euler Angles | float | (3) | Note the filter will create the Euler Angles array by interleaving the phi1, PHI and phi2 data arrays from the data file.  |
| **Cell Attribute Array**  | Pattern           | uint8_t   | (NxM) | The pattern data may be very large. There is an option to NOT read it into DREAM.3D if it is not needed by the analysis.   |


### Created Ensemble Attribute Arrays ###

These arrays will **most likely** be created but is not guaranteed. Additional arrays (unknown at the time of writing) may also be created.

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |
| **Ensemble Attribute Array** | LatticeConstants | float | (6) | The 6 values that define the lattice constants for each **Ensemble**|
| **Ensemble Attribute Array** | MaterialName | String | (1) | Name of each **Ensemble** |



## Example Pipelines ##


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


