Read H5EBSD File {#readh5ebsd}
======

## Group (Subgroup) ##
IO (Input)

## Description ##

This Filter reads from the **.h5ebsd** file that was generated with the [Import Orientation File(s) to H5EBSD](ebsdtoh5ebsd.html) filter. The user can use the checkboxes under the _**Voxel Data**_ tab to select which specific data arrays they are interested in processing. Different pipelines may require different arrays to be read and this advanced interface allows the user to be selective in the data that is read into memory for processing. The user can select a subset of the slices if they do not wish to process the entire volume of data. The type of transformations that are recommended based on the manufacturer of the data are also listed with a checkbox that the user can check to make sure the data is transformed into the proper _Euler_ and _Spatial_ reference frame.

If the processing pipeline is going to process phase based data for crystallographic information the user should enable the reading of the _CrystalStructure_ _**Ensemble Data**_ array.

-----

![Read H5Ebsd File User Interface](images/ReadH5Ebsd_UI.png)
@image latex ReadH5Ebsd_UI.png "Read H5Ebsd File User Interface" width=6in

-----

## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path to .h5ebsd |
| Start Slice | The first slice of data to read |
| End Slice | The last slice of data to read |
| Use Recommended Transformations | Boolean |
| Various Arrays | User selected arrays to read into memory |

## Required DataContainers ##

Voxel

## Required Arrays ##

None

## Created Arrays ##

An array for each of the arrays selected will be created, possibly over writing any arrays that were already in memory with the same name.


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


