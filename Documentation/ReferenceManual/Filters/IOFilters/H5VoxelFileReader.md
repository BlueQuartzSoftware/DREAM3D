Read Legacy DREAM.3D H5Voxel File {#h5voxelfilereader}
======

## Group/Subgroup ##
I/O Filters


## Description ##
This Filter reads legacy DREAM.3D file to obtain H5Voxel data.

## Parameters ## 
| Name | Type |
|------|------|
| Input File | File Path |

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the voxel/datapoint in Bunge convention (Z-X-Z) |  |
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint |  |
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. |  |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



