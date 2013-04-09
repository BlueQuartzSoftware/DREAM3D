Import Orientation File(s) to H5Ebsd {#ebsdtoh5ebsd}
======

## Group (Subgroup) ##
I/O Filters (Input)

## Description ##
This filter will convert orientation data in the form of EBSD style of data into a single file archive based on the [HDF5](http://www.hdfgroup.org) file specification. This filter is typically run as a single filter pipeline to do the conversion. All subsequent pipelines should use the [Read H5Ebsd Filter](readh5ebsd.html) as a data source instead of converting the data each time.

### Converting Orientation Data to H5EBSD Archive ###
In order to work with orientation data DREAM3D needs read the data from an archive file based on the HDF5 specification. In order to convert the data the user will first build a single filter pipeline by selecting the **Import Orientation File(s) to H5Ebsd** filter. This filter will convert a directory of sequentially numbered files into a single [HDF5](http://www.hdfgroup.org) file that retains all the meta data from the header(s) of the files. The user selects the directory that contains all the files to be imported then uses the additional input widgets on the filter user interface to make adjustments to the generated file name until the correct number of files is found. The user interface indicates to the user through red & green icons if all of the files exist on the file system and will also display a warning message at the bottom of the filter interface if any of the generated file names do not appear on the file system.

### Stacking Order ###
Due to different experimental setups the definition of the _bottom_ slice or the **Z=0** slice can be different. The user should verify that the proper button box is checked for their data set. 

+ **Low to High** This means that the file with the lowest index, closest to Zero (0), will be considered the Z=0 slice.
+ **High to Low** This means that the file with the highest index, farthest from Zero (0), will be considered the Z=0 slice.

### Z Resolution ###
Many serial sectioning systems are inherently a series of 2D scans stacked together to form a 3D volume of material. Due to this the experimental systems have no knowledge of the amount of material that was removed between each slice and so the user is responsible for setting this value correctly for their data set.

![Import Orientation Files User Interface](ImportOrientationDataFilter.png)

### Setting the Spatial Reference Frame ###
The user next item that the user needs to do is to click the _**Set Reference Frame**_ button to set the proper reference frame for the data set which will be written to the H5Ebsd file as meta data. Below are a number of examples showing the differences in the data sets that the different reference frames will have on the data set when the data is finally processed by DREAM3D.

![No Transform or Unknown Manufacturer](NoUnknown_RefFrame.png)

![TSL & HKL Transform](EDAX_HKL_RefFrame.png)

![HEDM Transform](HEDM_RefFrame.png)

### Supported File Formats ###
| Manufacturer  | File Extension | Comments |  
|---------------|----------------|----------|  
| EDAX  | .ang | --|  
| Oxford Instruments | .ctf | 2D data sets store angles in Degrees. 3D data sets store angles as Radians. The user needs to add the appropriate filter to convert the data if necessary |  
| HEDM from APS | .mic | A .config file with the same name is needed for each .mic file |

### Completing the Conversion ###
Once all the inputs are correct the user can click the _**Go**_ button to start the conversion. Progress will be displayed at the bottom of the DREAM3D user interface during the conversion.


## Parameters ##
See Notes.


## Required Arrays ##
None

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 2.0.0

**License:**  See the License.txt file that came with DREAM3D.



