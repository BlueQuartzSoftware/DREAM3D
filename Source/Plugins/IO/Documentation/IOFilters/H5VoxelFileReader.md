Read Legacy DREAM.3D H5voxel File {#h5voxelfilereader}
=======

## Group (Subgroup) ##
I/O Filters (Input)


## Description ##
This filter reads a legacy DREAM.3D file that holds the *Euler Angles*, *Phases* and *GrainIds* of each **Cell**.  The *Crystal Structures* are also read from the file.  While reading the file, the **Feature** map is sized to the largest *GrainId* observed.

## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |  |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** |  |
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. |  |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase is (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4)
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) |  |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

