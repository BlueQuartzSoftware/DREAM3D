Create Ensemble Info {#createensembleinfo}
=============

## Group (Subgroup) ##
Processing (Alignment)

## Description ##
This **Filter** reads in information about the crystal structure and phase types of all the **Features** that are contained in a **Cell** based volume. These values are needed to allow the calculation of statistics on the volume, if they have not already been provided by some other means. Each row in the table lists the __Crystal Structure__, __Phase Type__, and __Phase Name__. The proper values for the crystal structure and phase type come from internal constants within DREAM.3D and are listed here:

**Crystal Structure**

| String Name | Internal Value | Laue Name |
| ------------|----------------|----------|
| Hexagonal_High | 0 |  Hexagonal-High 6/mmm |
| Cubic_High | 1 |  Cubic Cubic-High m3m |
| Hexagonal_Low | 2 |  Hexagonal-Low 6/m |
| Cubic_Low | 3 |  Cubic Cubic-Low m3 (Tetrahedral) |
| Triclinic | 4 |  Triclinic -1 |
| Monoclinic | 5 |  Monoclinic 2/m |
| OrthoRhombic | 6 |  Orthorhombic mmm |
| Tetragonal_Low | 7 |  Tetragonal-Low 4/m |
| Tetragonal_High | 8 |  Tetragonal-High 4/mmm |
| Trigonal_Low | 9 |  Trigonal-Low -3 |
| Trigonal_High | 10 |  Trigonal-High -3m |
| UnknownCrystalStructure | 999 |  Undefined Crystal Structure |

**Phase Type**

| String Name | Internal Value |
| ------------|----------------|
| PrimaryPhase | 0 |
| PrecipitatePhase | 1 |
| TransformationPhase | 2 |
| MatrixPhase | 3 |
| BoundaryPhase | 4 |
| UnknownPhaseType | 999 |


## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Ensemble | Create Ensemble Info | The created Ensemble crystal structures, phase types, and phase names |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##
| Kind | Default Name | Type | Component Dimension | Description |
|------|--------------|-------------|---------|--------------|
| **Attribute Matrix** | EnsembleAttributeMatrix | Ensemble | N/A | Created **Ensemble Attribute Matrix** name |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |
| **Ensemble Attribute Array** | PhaseTypes        | uint32_t | (1) | Enumeration representing the phase type for each **Ensemble** |
| **String Data Array** 	   | PhaseNames        | String | (1)   | The phase names for each **Ensemble** |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


