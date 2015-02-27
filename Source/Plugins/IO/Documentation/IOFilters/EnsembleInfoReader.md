Read Ensemble Info File {#ensembleinforeader}
======
## Group (Subgroup) ##
IO Filters (Input)

## Description ##
This filter reads in information about the Crystal Structure and Phases of
 all the **Features** that are contained in a **Cell** based volume. These
 values are needed to allow the calculation of statistics on the volume.
 The format of that file is a simple text file with the extension .ini or .txt.
 The first group in the file  is the name [EnsembleInfo] in square brackets with the key Number_Phases=number of phases that are contained in the volume and subsequent groups in
 the file list the __Phase Number__, __Crystal Structure__ and __Phase Type__.
 The proper values for the crystal structure and phase type come from internal
 constants within DREAM3D and are listed here:

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

## Example ##

__Phase numbering starts at One (1). Phase Zero (0) is reserved for internal use in DREAM3D.__
For example if you have a structure that has 2 phases that consist of a Cubic Primary phase and a Hexagonal Matrix phase the file would be the following:


	[EnsembleInfo]
	Number_Phases=2

	[1]
	CrystalStructure=Cubic_High
	PhaseType=PrimaryPhase

	[2]
	CrystalStructure=Hexagonal_High
	PhaseType=MatrixPhase


## Parameters ##

| Name | Type |
|------|------|
| Input Ensemble Info File | Input File |

## Required DataContainers ##

A DataContainer to create the AttributeMatrix and DataArray

## Required Arrays ##

None

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | CrystalStructures | Integer [See above for values] |  |
| Ensemble | PhaseTypes        | Integer [See above for values] |  |



## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

