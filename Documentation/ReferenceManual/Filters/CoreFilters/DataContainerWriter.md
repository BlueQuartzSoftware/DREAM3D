Write DREAM3D Data File {#datacontainerwriter}
======

## Group (Subgroup) ##
IO Filters (Output)

## Description ##

This filter will write the contents of memory to an HDF5 based file with the file extension .dream3d. The user can specify which data containers to write
to the file. For each data container all attribute data will be written to the HDF5 file. The user can also specify that
an Xdmf (http://www.xdmf.org) file is written which allows loading of the data into ParaView in a much easier and
controlled fashion. See the Xdmf web site for more details on Xdmf. The Xdmf file will have the same name as the HDF5
file but with an .xdmf file extension.

### Example Output ###
The HDF5 based .dream3d file is a binary file.

The Xdmf file (partial):     

	<?xml version="1.0"?>
	<!DOCTYPE Xdmf SYSTEM "Xdmf.dtd"[]> 
	<Xdmf xmlns:xi="http://www.w3.org/2003/XInclude" Version="2.2">  
	 <Domain>
	  <!-- *************** START OF VolumeDataContainer *************** -->  
	  <Grid Name="VolumeDataContainer" GridType="Uniform">
	    <Topology TopologyType="3DCoRectMesh" Dimensions="118 202 190 "></Topology>
	    <Geometry Type="ORIGIN_DXDYDZ">
	      <!-- Origin  Z, Y, X -->  
	      <DataItem Format="XML" Dimensions="3">-29 0 -47</DataItem>
	      <!-- DxDyDz (Spacing/Resolution) Z, Y, X --> 
	      <DataItem Format="XML" Dimensions="3">0.25 0.25 0.25</DataItem>
	    </Geometry>
	    <Attribute Name="Confidence Index" AttributeType="Scalar" Center="Cell">
          <DataItem Format="HDF" Dimensions="117 201 189 1" NumberType="Float" Precision="4" >
            Untitled1.dream3d:/DataContainers/VolumeDataContainer/CellData/Confidence Index
          </DataItem>
        </Attribute>
	…..  


## Parameters ##

| Name | Type |
|------|------|
| Output File | .dream3d |
| Write Xdmf File | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##
None


## Created Arrays ##
None



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


