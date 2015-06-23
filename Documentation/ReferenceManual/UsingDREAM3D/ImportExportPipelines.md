Saving & Opening Pipelines {#importexportsavepipeline}
=========

A current **Pipeline** in the **Pipeline View** can be saved to the disk by selecting the _Save_ option in the _File_ menu. Once saved to the disk, the files can be **Bookmarked** for quick access. For more information on **Bookmarks**, visit the [user interface](@ref userinterface) section. A saved **Pipeline** file can be opened by choosing the _Open..._ option in the _File_ menu and selecting the desired **Pipeline** file. 

DREAM.3D utilizes a _document_ model when dealing with saving and opening **Pipelines**. Whenever a **Pipeline** file is opened, it will appear in a new DREA.3D window. If the **Pipeline** is modified and the user selects the _Save_ option, the **Pipeline** file will be updated with the changes. 

## Pipeline File Formats ##
DREAM.3D allows the user to _export_ and _import_ **Pipelines** to a variety of file formats. The currently supported file formats are

+ .json
+ .dream3d
+ .txt
+ .ini

Versions of DREAM.3D prior to version 6 saved **Pipelines** in the .ini format, which may have ended with either the .txt or .ini file extensions. The current version of DREAM.3D is able to read these older formats, but all new **Pipelines** will be saved to either the .json or .dream3d file format. The information written to a **Pipeline** file includes:

+ The list of **Filters** in the **Pipeline**, in sequential order
+ All the variables for each **Filter** and their settings
+ The name of the **Pipeline** file
+ The version of DREAM.3D used to create the **Pipeline**


-------

	{
  	  	"0": {
    	    "CellAttributeMatrixName": "CellData",
        	"CellEnsembleAttributeMatrixName": "CellEnsembleData",
    	    "DataContainerName": "DataContainer",
    	    "FilterVersion": "5.2.1285",
    	    "Filter_Human_Label": "Read H5Ebsd File",
    	    "Filter_Name": "ReadH5Ebsd",
    	    "InputFile": "Data/Output/SmallIN100.h5ebsd",
    	    "RefFrameZDir": 1,
    	    "SelectedArrayNames": [
    	        "Image Quality",
    	        "Phases",
    	        "Confidence Index",
    	        "EulerAngles"
    	    ],
    	    "UseTransformations": 1,
    	    "ZEndIndex": 117,
    	    "ZStartIndex": 1
    	},
    	"1": {
    	    "FilterVersion": "5.2.1285",
    	    "Filter_Human_Label": "Write DREAM3D Data File",
    	    "Filter_Name": "DataContainerWriter",
    	    "OutputFile": "Data/Output/SmallIN100.dream3d",
    	    "WriteXdmfFile": 1
    	},
    	"PipelineBuilder": {
    	    "Name": "(02) Small IN100 Initial Visualization",
    	    "Number_Filters": 2,
    	    "Version": "5.2.1285"
    	}
	}

<center><b>An Example Pipeline in .json Format</b></center>

-------

The .json file format is an open standard that most scripting languages, such as Python and MATLAB, can easily understand. Therefore, it is straightforward to parse the .json file with an external program to make modifications as necessary. You can even set up scripting to create many version of a **Pipeline** file and execute them from the command line using the [PipelineRunner](@ref pipelinerunner) tool.

The .dream3d file format is a binary file based on [HDF5](https://www.hdfgroup.org/HDF5/). More specific information about this file format can be found in the [Native DREAM.3D File Format](@ref nativedream3d) section. The .dream3d format is also how the data structure objects are stored to disk. Whenever a user writes a new DREAM.3D file, the **Pipeline** used to create it will be written into the .dream3d file alongside the data structure objects. This means that the data structure is always accompanied by the workflow that was used to create it, adding _provenance_ to the data. If you select a .dream3d file from the _Open_ option of the _File_ menu, the **Pipeline** from within that .dream3d file will be extracted and opened in a new window. This allows users to recover the workflow used to create a particular DREAM.3D file. Additionally, a .dream3d file can be dragged from the file system and dropped into the **Pipeline View**. The user is then asked if they would like to extract the **Pipeline** from the file or read the data structure objects out of the file into the current **Pipeline**. 

Note that if you select to save a **Pipeline** file as a .dream3d file, the resulting .dream3d file will **only** have the current **Pipeline** in it, not any of the data structure. To save the actual data structure along with the **Pipeline**, you must run the [Write DREAM.3D Data File](@ref datacontainerwriter) **Filter**.