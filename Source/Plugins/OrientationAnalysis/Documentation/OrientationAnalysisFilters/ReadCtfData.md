Read Oxford Instr. Ebsd Data {#readctfdata}
=============

## Group (Subgroup) ##

IO (Input)

## Description ##

This **Filter** will read a single .ctf file into a new **Data Container** with a corresponding **Image Geometry**, allowing the immediate use of **Filters** on the data instead of having to generate the intermediate .h5ebsd file. A **Cell Attribute Matrix** and **Ensemble Attribute Matrix** will also be created to hold the imported EBSD information. Currently, the user has no control over the names of the created **Attribute Arrays**. The user should be aware that simply reading the file then performing operations that are dependent on the proper crystallographic and sample reference frame will be **undefined, inaccurate and/or wrong**. In order to bring the [crystal reference frame](@ref rotateeulerrefframe) and [sample reference frame](@ref rotatesamplerefframe) into coincidence, rotations will need to be applied to the data. An excellant reference for this is the following PDF file:
[http://pajarito.materials.cmu.edu/rollett/27750/L17-EBSD-analysis-31Mar16.pdf](http://pajarito.materials.cmu.edu/rollett/27750/L17-EBSD-analysis-31Mar16.pdf)

### Default HKL Transformations ###

If the data has come from a HKL acquisition system and the settings of the acquisition software were in the default modes, then the following reference frame transformations need to be performed:

+ Sample Reference Frame: 180<sup>o</sup> about the <010> Axis
+ Crystal Reference Frame: None

The user also may want to assign un-indexed pixels to be ignored by flagging them as "bad". The [Threshold Objects](@ref multithresholdobjects) **Filter** can be used to define this _mask_ by thresholding on values such as _Error_ = 0.


### Radians and Degrees ###

2D .ctf files have their angles in **degrees**. Please be sure to [convert](@ref changeanglerepresentation) the Euler angles from degrees to radians before running any other **Filter**.


### The Axis Alignment Issue for Hexagonal Symmetry [1] ###

+ The issue with hexagonal materials is the alignment of the Cartesian coordinate system used for calculations with the crystal coordinate system (the Bravais lattice).
+ In one convention (e.g. EDAX.TSL), the x-axis, i.e. [1,0,0], is aligned with the crystal a1 axis, i.e. the [2,-1,-1,0] direction. In this case, the y-axis is aligned with the [0,1,-1,0] direction. (Green Axis in Figure 1)
+ In the other convention, (e.g. Oxford Instr, Univ. Metz software), the x-axis, i.e. [1,0,0], is aligned with the crystal [1,0,-1,0] direction. In this case, the y-axis is aligned with the [-1,2,-1,0] direction. (Red Axis in Figure 1)
+ This is important because texture analysis can lead to an ambiguity as to the alignment of [2,-1,-1,0] versus [1,0,-1,0], with apparent **30°** shifts in the data.
+ Caution: it appears that the axis alignment is a choice that must be made when installing TSL software so determination of which convention is in use must be made on a case-by-case basis. It is fixed to the y-convention in the HKL software.
+ The main clue that something is wrong in a conversion is that either the 2110 & 1010 pole figures are transposed, or that a peak in the inverse pole figure that should be present at 2110 has shifted over to 1010.
+ DREAM.3D uses the TSL/EDAX convention.
+ **The result of this is that the filter will _AUTOMATICALLY_ add 30° to phi2 when reading Oxford Instr (.ctf) files. There is no way to turn off this behavior. **

| Figure 1 |
|--------|
| ![](images/Hexagonal_Axis_Alignment.png) |
| **Figure 1:** showing TSL and Oxford Instr. conventions. EDAX/TSL is in **Green**. Oxford Inst. is in **Red** |





## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path |The input .ctf file path |

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
| **Cell Attribute Array** | BC           | float | (1) | Band contrast levels |
| **Cell Attribute Array** | EulerAngles  | float | (3) | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z)  |
| **Cell Attribute Array** | BS           | float | (1) | Band saturation levels |
| **Cell Attribute Array** | Bands        | float | (1) | Number of bands   |
| **Cell Attribute Array** | Phases       | int32_t   | (1) | Specifies to which phase each **Cell** belongs   |
| **Cell Attribute Array** | MAD          | float | (1) | Mean angular deviation. Typical threshold value is < 1.8  |
| **Cell Attribute Array**  | X Position       | float |(1) | X coordinate of **Cell**   |
| **Cell Attribute Array**  | Y Position       | float |(1) | Y coordinate of **Cell**   |
| **Cell Attribute Array** | Error        | int32_t   | (1) | Value = 0 is a well indexed scan point   |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |
| **Ensemble Attribute Array** | LatticeConstants | float | (6) | The 6 values that define the lattice constants for each **Ensemble**|
| **Ensemble Attribute Array** | MaterialName | String | (1) | Name of each **Ensemble** |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## References ##

[1] Rollett, A.D. Lecture Slides located at [http://pajarito.materials.cmu.edu/rollett/27750/L17-EBSD-analysis-31Mar16.pdf](http://pajarito.materials.cmu.edu/rollett/27750/L17-EBSD-analysis-31Mar16.pdf)

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

