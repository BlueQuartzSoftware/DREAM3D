Read H5Ebsd File {#readh5ebsd}
======

## Group (Subgroup) ##
I/O Filters (Input)


## Description ##

This Filter reads from the **.h5ebsd** file that was generated with the [Import Orientation File(s) to H5Ebsd](ebsdtoh5ebsd.html) filter. The user can use the checkboxes under the _**Voxel Data**_ tab to select which specific data arrays they are interested in processing. Different pipelines may require different arrays to be read and this advanced interface allows the user to be selective in the data that is read into memory for processing. The user can select a subset of the slices if they do not wish to process the entire volume of data. The type of transformations that are recommended based on the manufacturer of the data are also listed with a checkbox that the user can check to make sure the data is transformed into the proper _Euler_ and _Spatial_ reference frame.

If the processing pipeline is going to process phase based data for crystallographic information the user should enable the reading of the _CrystalStructure_ _**Ensemble Data**_ array.

![Read H5Ebsd File User Interface](images/ReadH5Ebsd_UI.png)
@image latex ReadH5Ebsd_UI.png "User Interface" width=6in


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

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

