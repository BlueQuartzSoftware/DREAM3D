Write DREAM3D Data File {#datacontainerwriter}
======

## Group (Subgroup) ##
IO Filters (Output)

## Description ##

This filter will write the contents of memory to an HDF5 based file. The user can specify which data containers to write
to the file. For each data container all attribute data will be written to the HDF5 file. The user can also specify that
an Xdmf (http://www.xdmf.org) file is written which allows loading of the data into ParaView in a much easier and
controlled fashion. See the Xdmf web site for more details on Xdmf. The Xdmf file will have the same name as the HDF5
file but with an .xdmf file extension.


## Parameters ##

| Name | Type |
|------|------|
| Output File | Output File |
| Write Cell Data | Boolean (On or Off) |
| Write SurfaceMesh Data | Boolean (On or Off) |
| Write Xdmf File | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##
None


## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

