Xdmf for Visualization {#xdmf}
========

[Xdmf](http://www.xdmf.org) is an XML file that describes either a regular grid or an unstructured grid data set where the data can be stored in an [HDF5](http://www.hdfgroup.org) file, binary file or with the XML file itself. The user can find out more information for this format at the [Xdmf website](http://www.xdmf.org). DREAM.3D will optionally write out an Xdmf file when writing a DREAM.3D data file so that users can view the processed data easily using the open source [ParaView](http://www.paraview.org) application. In prior versions of DREAM.3D, the only way to view the data was to write out a legacy VTK file, which can be slow and duplicate much of the data that is stored in the .dream3d file. The Xdmf format allows for more efficient approach to visualization

-------

![Opening an Xdmf File in ParaView](Images/XdmfParaViewLoading.png)
@image latex Images/XdmfParaViewLoading.png "Opening an Xdmf File in ParaView" width=3in

--------

By using an Xdmf wrapper file, the user can selectively load just the data that they are interested in viewing. This saves time loading the data, and, more importantly, reduces the amount of memory needed to load, store and view the data.

*Note:* It is a **REQUIREMENT** that the Xdmf file and the DREAM.3D file are in the same directory, because paths in the Xdmf file refer to data stored in the DREAM.3D file. The XML files are typically a few kilobytes in size and are, therefore, a huge savings over writing the VTK file for the same data. Also, **DO NOT CHANGE THE NAME OF A DREAM.3D OR ITS CORRESPONDING XDMF FILE!** The data will always be safe, but changing the names of the files disrupts the paths in the Xdmf files. If you would change the name of a DREAM.3D file, simply re-import into the DREAM.3D application and immediately export it again with a new name.

## Xdmf Example Contents ##
Below is an example of what an Xdmf file might contain:

    <?xml version="1.0"?>
    <!DOCTYPE Xdmf SYSTEM "Xdmf.dtd"[]>
    <Xdmf xmlns:xi="http://www.w3.org/2003/XInclude" Version="2.2">
     <Domain>
      <Grid Name="Volume DataContainer" GridType="Uniform">
        <Topology TopologyType="3DCoRectMesh" Dimensions="183 885 1025 "></Topology>
        <Geometry Type="ORIGIN_DXDYDZ">
          <!-- Origin -->
          <DataItem Format="XML" Dimensions="3">0 0 0</DataItem>
          <!-- DxDyDz (Spacing/Resolution)-->
          <DataItem Format="XML" Dimensions="3">1 1 1</DataItem>
        </Geometry>
        <Attribute Name="ImageData" AttributeType="Scalar" Center="Cell">
          <DataItem Format="HDF" Dimensions="182 884 1024 " NumberType="UChar" Precision="1" >
            out.dream3d:/Volume DataContainer/CELL_DATA/ImageData
          </DataItem>
        </Attribute>
      </Grid>    
     </Domain>
    </Xdmf>
