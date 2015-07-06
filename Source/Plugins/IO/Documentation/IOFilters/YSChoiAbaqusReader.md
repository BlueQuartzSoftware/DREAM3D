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

## Required Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|

## Created Objects ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |  |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** |  |
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. |  |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | The first value is a dummy value, so each **Cell** has quat = {dummy, q1, q2, q3, q4} - where q1, q2, and q3 contain the axis information and q4 contains the angle information of the quaternion |
| Feature | SurfaceFeatures | Boolean flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not. |  |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) |  |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


