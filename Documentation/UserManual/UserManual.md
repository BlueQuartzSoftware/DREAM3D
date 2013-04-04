DREAM3D User Manual {#usermanual}
========

## Acknowledgements / Funding ##
  Parts of DREAM3D were written under US Air Force (AFRL) Contract FA8650-07-D-5800 and Navy Research Labs (NRL) Contract N00173-07-C-2068. Many others have contributed to the code including the following:

### AFRL ###
![](Images/AFRL_Logo.png)
Dr. Michael A. Groeber

### BlueQuartz Software ###
![](Images/BlueQuartzSoftware_Logo.png)
Mr. Michael A. Jackson

### Carnegie Mellon University ###
![](Images/CMU_MRSEC_Logo.png)

+ Dr. Tony Rollett
+ Dr. Greg Rohrer
+ Dr. Marc De Graef

### Code Developers & Contributors ###

+ Dr. Sukbin Lee
+ Dr. Joe Tucker
+ Dr. Lisa Chan
+ Dr. Stephen Sintay
+ Dr. Abhijeet Brahme
+ Dr. David Saylor
+ Dr. Joe Fridy
+ Dr. Patrick Callahan
+ Dr. Jarrell Waggoner
+ Dr. Megna Shah

### Vision ###


+ Dr. Somnath Ghosh
+ Dr. Chris Woodward
+ Dr. Tony Rollett

### Initial Work Completed Under ###
Dr. Somnath Ghosh (The Ohio State University)

### Introduction ### 
Welcome to DREAM.3D, an open and modular software package that allows users to reconstruct, instantiate, quantify, mesh, handle and visualize microstructure digitally. The core software package has four (4) major functionalities. First, a series of EBSD data files (either TSL .ang or HKL .ctf) can be read and packed into a single HDF5 data file in order to reduce data size and clutter.  Second, the sections can be aligned, cleaned, reconstructed, segmented and statistically characterized. Third, the statistics can be read in and used to generate statistically equivalent, synthetic microstructures. Fourth, the reconstructed volumes and the synthetic volumes can be surfaced meshed using a multi-material marching cubes algorithm.

### DREAM3D User Interface ### 
Locate the DREAM3D application and open the application in the normal method for your operating system. After DREAM3D successfully opens the user is presented with the user interface presented in figure 3.1. There are 4 main areas of the window which are:


![](Images/UI_1.png)


#### The Filter Library | Prebuilt Pipelines | Favorite Pipelines #### 
  **Area 1**: This area of the user interface is broken into 3 basic groups: Filter Library, Prebuilt Pipelines and Favorite pipelines. The _Filter Library_ section contains the various groups that the filters are organized into. The user can click a specific group and just the filters associated with that group will be displayed in the _Filter List_ area of the user interface. DREAM3D provides several _Prebuilt Pipelines_ that can aid the new user in getting started with DREAM3D. Simply double clicking a preset will clear any current filters in the pipeline area and populate the pipeline area with the filters from the Pipeline Preset. After the user builds a pipeline that they may want to save for later the user can click the "Add Favorites" button to save that specific pipeline configuration.

#### Filter List #### 
  **Area 2**:This section lists the filters that are associated with a specific group that is selected in the _Filter Library_ area. If the _Filter Library_ is itself selected then all filters will be shown in this list.


#### Pipeline Area #### 
  **Area 3**:This area is where the user will construct their pipeline by either double clicking on a filter in the library area or dragging a filter from the Filter List and dropping the filter into the pipeline area. Filters in this section can be rearranged by simply dragging the filter into a new location.


####  Errors area #### 
 **Area 4**: This area displays any errors associated with the filter if it is actively being used in a pipeline. The display of the errors can be toggled on and off by pressing the appropriate buttons.

![](Images/UI_2.png)


### Building a Pipeline ### 
In order the build a pipeline the user can either double click on a particular filter or drag the filter from the _Filter List_ into the _Pipeline Area_.


![](Images/UI_3.png)

  After placing a filter into the Pipeline area the user interface will look like the next figure. The user should note the red outline of the filter widget which indicates that one or more of the input parameters has an error. By looking at the error table the user can read the error message from the pipeline and act accordingly. In the figure shown the error message is indicating that the input file for the filter is missing or has not been filled into the Text Edit (the widget with the Red outline).


![](Images/UI_4.png)


The user can place multiple filters into the pipeline to generate a workflow of data and analysis filters culminating in the output of one or several data files. The user can reorder the filters by simply using drag-and-drop of a selected filter in the pipeline and moving it into a new position in the pipeline. As filters are placed into the pipeline a basic error checking algorithm called the **Preflight** is executed on the entire pipeline. During the preflight stage the pipeline will have each filter check its input parameters and the availability needed data arrays. If the user places a filter that requires data that is **not** available an error will be added to the error table. The user will need to insert a filter into the pipeline before the error'ed filter that generates the missing data array.

### Example Pipelines ### 
DREAM3D is capable of a wide variety of reconstruction, data analysis and synthetic microstructure which can present issues to the new user with regards to how to get started contructing a working pipeline. In the section a few sample pipelines are presented which will hopefully allow the new user to get started quickly.


#### EBSD Reconstruction #### 
With the advent of 3D EBSD data collection systems the visualization of the collected data is typically the first order of business. Using the sample data set _Small IN100_ which is a Nickel based super alloy we can use DREAM3D to build a 3D reconstruction of the 2D slices. The order of operations for this type of work flow is the following:

+ Read H5Ebsd file
+ Determine which voxels are considered _good_.
+ Align the sections using a Misorientation Alignment filter
+ Segment the grains using a Misorientation Algorithm
+ Generate the IPF (Inverse Pole Figure) colors based on the Euler Angles
+ Write out a DREAM3D data file (with optional Xdmf file>


![](Images/ex_reconstruction.png)

After running the pipeline ParaView can be used to display the generated VTK file.

![](Images/ex_reconstruction_2.png)

