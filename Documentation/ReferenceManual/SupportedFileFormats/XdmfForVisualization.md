XDMF For Visualization {#xdmf}
========

## Introduction ##
[XDMF](http://www.xdmf.org) is an XML file that describes either a regular grid or an unstructured grid data set where the data can be stored in an HDF5, Binary or with the xml file itself. The user can find out more information for this format at the [Xdmf website](http://www.xdmf.org). DREAM3D will optionally write out an Xdmf file when writing a DREAM3D Data file so that users can view the processed data easily using the free [ParaView](http://www.paraview.org) application. In prior versions of DREAM3D, the only way to view the data was to write out a legacy VTK file, which can be slow and simply duplicates much of the data that is stored in the .dream3d file. This is a waste of space and is not memory efficient when it comes to ParaView loading the data from the legacy .vtk file.

-------

![Opening an Xdmf File in ParaView](Images/XdmfParaViewLoading.png)

@image latex Images/XdmfParaViewLoading.png "Opening an Xdmf File in ParaView" width=3in

--------

By using an Xdmf wrapper file the user can selectively load just the data that they are interested in viewing. This saves time loading the data, and, more importantly, reduces the amount of memory needed to load, store and view the data.

**Note**: It is a REQUIREMENT that the Xdmf file and the DREAM3D file are in the same directory, because paths in the XDMF file refer to data stored in the DREAM3D file. The XML files are typically a few kilobytes in size and are, therefore, a huge savings over writing the VTK file for the same data.

## XDMF Example Contents ##
Below is an example of what an Xdmf file might contain.

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
