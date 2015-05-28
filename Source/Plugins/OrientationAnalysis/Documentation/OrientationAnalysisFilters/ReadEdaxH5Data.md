Read EDAX/TSL Ebsd Data (HDF5 Format) {#readedaxh5data}
=====


## Group (Subgroup) ##
IOFilters (Input)


## Description ##
This filter will read a single .h5 file into a DataContainer backed by an ImageGeometry allowing the immediate use of filters on the data instead of having to generate the intermediate .h5ebsd file. The user should be aware that simply reading the file then performing operations that are dependent on the proper crystallographic and sample reference frame will be undefined or simply **wrong**. In order to bring the crystal reference frame and sample reference frame into coincidence the proper filters need to be run. The user should read the documentation for the "[Rotate Sample Reference Frame](rotatesamplerefframe.html)" and "[Rotate Euler Reference Frame](rotateeulerrefframe.html)".

### Default TSL Transformations ###

If the data has come from a TSL based acquisition system and the settings of the acquisition software were in the default modes then the following reference frame transformations may need to be performed based on the version of the OIM Analysis software being used to collect the data. The user is **strongly** encourages to seek the advice of the collecting personnel to verify their equipment setup. Newer versions of OIM Analysis have changed the defaults which may render the reference frame transformations unneeded. 

+ Sample Reference Frame: 180 Degrees about the <010> Axis
+ Crystal Reference Frame: 90 Degrees about the <001> Axis

The user also may want to assign unindexed pixels to be ignored and be assigned an RGB Color of Black. In this case the user can insert a [Threshold Objects](multithresholdobjects.html) filter to define the "Good Voxels" cell data. Typical thresholds include _Confidence Index_ > 0.1 or _Image Quality_ > some user defined value.

## Parameters ##

| Name             | Type |
|------------------|------|
| Input File | The Path to the .h5 file |
| Scan Name | The name of the scan in the hdf5 file. EDAX can store multiple scans in a single file |

## Required Arrays ##

None

## Required DataContainers ##

The filter will create a new DataContainer with an ImageGeometry object.


## Created Arrays ##

Arrays are created based on the data read from the EBSD file. The following table lists the arrays that are read from the HDF5 based EDAX Ebsd file. 

| Type | Array Name | Component Dimensions | Comment |
|------|--------------------|-------------|---------|
| Float  | Confidence Index | [1]     | Ranges from 0.0 to 1.0   |
| Float  | Euler Angle      | [3]     |    |
| Float  | Fit              | [1]     |    |
| Float  | Image Quality    | [1] |    |
| Int    | Phases             | [1] | Numbered starting from 1 for a valid phase   |
| Float  | SEM Signal       | [1] |    |
| Float  | X Position       | [1] |    |
| Float  | Y Position       | [1] |    |



## Authors ##

**Copyright** 2015 BlueQuartz Software

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


