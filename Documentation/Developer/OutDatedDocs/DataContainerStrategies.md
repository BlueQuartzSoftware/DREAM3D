DataContainer Use in Filters {#DataContainerStrategies}
==========

##  Introduction
The DataContainer class within the DREAM3D development framework is designed to hold in memory all the data associated with a reconstructed microstructure. As
such it is built in a way to hold generic high level C++ classes. For this the
DataContainer is broken into a few categories:
- Voxel Data
- Feature Data
- Ensemble Data
 _Note; Need explanations here_

The programmer can store multiple Arrays for each of the categories that describe the data being processed. For example there is a standard array of integers that describe the "Grain ID" for each voxel and stored in an array called "GrainIds".

## Filter Design
All filters must inherit from the _AbstractFilter_ class and implement the _virtual void execute()_ method. Each AbstractFilter has a method to retrieve the DataContainer object assigned to that particular filter. It is recommended that the first bit of code within the _execute()_ method be the following:
  
    DataContainer* m = getDataContainer();
    if(NULL == m)
    {
        setErrorCondition(-1);
        std::stringstream ss;
        ss << getNameOfClass() << " DataContainer was NULL";
        setErrorMessage(ss.str());
        return;
    }
    int err = 0;
    setErrorCondition(err);

This will ensure that a valid DataPointer object is available for use during the filter. It is at this point that the programmer needs to decide in what manner they will store references to any data arrays from the DataContainer. If all of the filter code will be within the _execute_ method then using local variables is the way to go. An example of this is the following:

    int64_t totalPoints = m->totalPoints();
    int32_t* grain_indicies =
    m->getVoxelDataSizeCheck<int32_t, Int32ArrayType, AbstractFilter>(DREAM3D::VoxelData::GrainIds, totalPoints, this);
    if(NULL == grain_indicies)
    {
        return;
    }

Here we are using a template function from the DataContainer class that will ensure the array is properly extracted from the DataContainer class and returns a pointer to the internal memory location that holds the actual data. The template function is parameterized on the pointer type, DataArray Type and the class that inherits from the _Observable_ class, which _AbstractFilter_ does.

This _getVoxelDataSizeCheck()_ method first checks to make sure the data array is available, and if so checks the size of the array to make sure it matches what the programmer thinks it should be.

## Checking for Required Arrays

In many filters that are written the filter itself is modifying or reading data from existing arrays. If this is the case there are several preprocessor macros defined in _DREAM3DLib/Common/DataContainerMacros.h_. The first macro to discuss checks for the availability of an array by name in the DataContainer. In the next example we are going to get the GrainIds array by name and store the pointer to the data in a private instance variable called "m_GrainIds". This macro also uses the instance variable called GrainIdsArrayName. That is a lot of information so lets take it one piece at a time.

Here is the use of the Macro.

    GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)


**Required instance Variables**
- int32_t* m_GrainIds;
- std::string m_GrainIdsArrayName;

In the initializer list of the filter constructor we should be initializing these variables to known values. For example in the DxWriter class this is the class constructor:


    DxWriter::DxWriter() :
    FileWriter(),      // We inherit from FileWriter
    m_GrainIdsArrayName(DREAM3D::CellData::GrainIds), // Set to known value
    m_AddSurfaceLayer(false),
    m_GrainIds(NULL) // Set to NULL
    {
      setupFilterOptions();
    }


## Creating Arrays to Store in the DataContainer


    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)
 
