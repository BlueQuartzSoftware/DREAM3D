Write DREAM.3D Data File {#datacontainerwriter}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##

This **Filter** will write the contents of the current data structure to an [HDF5](https://www.hdfgroup.org/HDF5/) based file with the file extension .dream3d. The user can specify whether to write an [Xdmf](http://www.xdmf.org) that allows loading of the data into [ParaView](http://www.paraview.org/) for visualization. 

For more information on these outputs, see the [file formats](@ref supportedfileformats) documentation.


## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Output File | File Path | The outpute .dream3d file path |
| Write Xdmf File (ParaView Compatible File) | bool | Whether to write an Xdmf file for visualization |
 

## Required Geometry ##
Not Applicable

## Required Objects ##
None


## Created Objects ##
None



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


