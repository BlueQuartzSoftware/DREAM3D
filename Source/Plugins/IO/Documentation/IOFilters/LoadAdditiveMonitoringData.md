Read Ensemble Info File {#ensembleinforeader}
======
## Group (Subgroup) ##
IO Filters (Input)

## Description ##
This filter reads in information about the Crystal Structure and Phases of
 all the **Features** that are contained in a **Cell** based volume. These
 values are needed to allow the calculation of statistics on the volume.
 The format of that file is a simple text file where the first line is the
 number of phases that are contained in the volume and subsequent lines in
 the file list the __Phase Number__, __Crystal Structure__ and __Phase Type__.
 The proper values for the crystal structure and phase type come from internal
 constants within DREAM3D and are listed here:

**Crystal Structure**

- Hexagonal = 0
- Cubic = 1
- OrthoRhombic = 2
- AxisOrthoRhombic = 3
- UnknownCrystalStructure = 999

**Phase Type**

- PrimaryPhase = 0
- PrecipitatePhase = 1
- TransformationPhase = 2
- MatrixPhase = 3
- BoundaryPhase = 4
- UnknownPhaseType = 999
__Phase numbering starts at One (1). Phase Zero (0) is reserved for internal use in DREAM3D.__
For example if you have a structure that has 2 phases that consist of a Cubic Primary phase and a Hexagonal Matrix phase the file would be the following:


    2
    1  1  0
    2  0  3


## Parameters ##

| Name | Type |
|------|------|
| Input Ensemble Info File | Input File |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) |  |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

