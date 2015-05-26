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

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


