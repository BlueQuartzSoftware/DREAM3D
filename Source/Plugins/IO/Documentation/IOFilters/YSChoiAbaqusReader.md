Read YS Choi Abaqus Vtk Output File {#yschoiabaqusreader}
======
## Group (Subgroup) ##
I/O Filters

## Description ##

## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |  |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** |  |
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. |  |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | The first value is a dummy value, so each **Cell** has quat = {dummy, q1, q2, q3, q4} - where q1, q2, and q3 contain the axis information and q4 contains the angle information of the quaternion |
| Feature | SurfaceFeatures | Boolean flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not. |  |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

