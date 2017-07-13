Writing A Filter {#writingafilter}
========

Developers can write new filters for DREAM.3D to accomplish their own objectives.  There are several major functions that every new filter must implement to work with DREAM.3D's filter interface.

A DREAM.3D filter's major functions are [setupFilterParameters()](#setupFilterParameters), [preflight()](#preflight), [dataCheck()](#dataCheck), [execute()](#execute), and [Getter Functions](#getterfunctions)

## Major Functions ##
<a name="setupFilterParameters">

### setupFilterParameters() ###

</a>

**setupFilterParameters()** is where the filter instantiates its filter parameters.  Filter parameters are used as a way to set values into properties either in the DREAM.3D user interface or some other algorithm.  To learn more about filter parameters and the different available types, see [Filter Parameters](#filterparameters).

All filter parameters that are instantiated in this method are stored in a FilterParameterVector, and then set into the filter's *FilterParameters* property.

Here's an example that shows a basic setupFilterParameters function:

```
void SimpleExampleFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Number of Circles", NumberOfCircles, FilterParameter::Parameter, SimpleExampleFilter));
  setFilterParameters(parameters);
}
```
*Example 1*

In Example 1, we are instantiating a filter parameter that controls an integer property, *NumberOfCircles*.  The SIMPL_NEW_INTEGER_FP macro is a short-form way of instantiating a filter parameter that controls an integer property.

There are 4 required parameters and 1 optional parameter that are always passed to the SIMPL_NEW_INTEGER_FP macro in the following order:
`HumanLabel, PropertyName, Category, FilterName (class name), GroupIndex (optional).`

Therefore, as a concrete example, the macro should be written like this:
`SIMPL_NEW_INTEGER_FP("HumanLabel", PropertyName, Category, FilterName, GroupIndex)`

The **HumanLabel** is the name of the property, as it appears to DREAM.3D users. (*Number of Circles*)
The **PropertyName** is the internal name of the controlled property; this name must match a property defined in the header of the filter. (*NumberOfCircles*)
The **Category** is the categorical group box that the filter parameter's corresponding widget will be placed in in DREAM.3D. (*FilterParameter::Parameter*)
The **FilterName** is the class name of the Filter that this filter parameter is instantiated within. (*ExampleFilter*)
The **GroupIndex** is an integer that specifies the grouping of this filter parameter.  This parameter is optional, and is not used in the above example.

Here's a more complicated example with multiple filter parameters:

```
void ComplicatedExampleFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  
  parameters.push_back(SIMPL_NEW_STRING_FP("Output Attribute Array", NewCellArrayName, FilterParameter::CreatedArray, ComplicatedExampleFilter));
  parameters.push_back(SIMPL_NEW_DOUBLE_FP("Maximum Radius", MaxRadius, FilterParameter::Parameter, ComplicatedExampleFilter));
  
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int8, 1, SIMPL::AttributeMatrixObjectType::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Attribute Array to Process", SelectedCellArrayPath, FilterParameter::RequiredArray, ComplicatedExampleFilter, req));
  }

  setFilterParameters(parameters);
}
```
*Example 2*

In Example 2, we are instantiating three filter parameters; the first one controls the string property *NewCellArrayName*, the second one controls the double property *MaxRadius*, and the third one controls the DataArrayPath property *SelectedCellArrayPath*.

String and double filter parameters are instantiated with the same parameter list as integer filter parameters:
`HumanLabel, PropertyName, Category, FilterName (class name), GroupIndex (optional).`

In this example, however, the macro SIMPL_NEW_DA_SELECTION_FP creates a filter parameter that controls a DataArrayPath property that the user selects.

The SIMPL_NEW_DA_SELECTION_FP macro should be written in this form:
`SIMPL_NEW_DA_SELECTION_FP("HumanLabel", PropertyName, Category, FilterName, RequirementType, GroupIndex)`

SIMPL_NEW_DA_SELECTION_FP's additional parameter, *RequirementType*, determines which data arrays are available to be selected.  Any data arrays that do not conform to the specified RequirementType are greyed-out during data array selection in the DREAM.3D user interface.

The order that the filter parameters are pushed into the FilterParameterVector determines the order that they appear in the DREAM.3D user interface.

<a name="preflight">

### preflight() ###

</a>

DREAM.3D uses a process called **preflight** to check its current pipeline for errors and warnings whenever any of the filters or values in the current pipeline have changed.  The **preflight** process analyzes the current pipeline for setup errors and other mistakes before the user is given the option to execute the pipeline.

Each filter has its own **preflight()** function, which gets executed whenever preflight runs.

Example **preflight()** function:
  ```
void ExampleFilter::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}
  ```
*Example 3*

The **setInPreflight(true)** function marks the filter as "in preflight".
The signal **preflightAboutToExecute()** is emitted so that various operations can be performed before preflight occurs on the filter.
The signal **updateFilterParameters(this)** is emitted and it invokes a slot that sets the values from the user interface into the filter.
The **dataCheck()** function actually preflights the data in the filter, and is discussed below in greater detail.
The signal **preflightExecuted()** is emitted so that various operations can be performed after the filter's preflight is finished.
The **setInPreflight(false)** function marks the filter as no longer "in preflight".

Example 3 shows the necessary preflight code that should be written in every filter; developers can add extra code to the **preflight()** function if they wish.

<a name="dataCheck">

### dataCheck() ###

</a>

The **dataCheck()** function is the section where the filter's developer can write all the sanity checks for the filter to make sure that the filter will run smoothly and without errors.  These checks will be executed when preflight runs on the filter, and errors and warnings will appear if any of the checks fail.

Some examples of common sanity checks:
- Created arrays have been given unique names.
- Arrays needed by the filter currently exist when this filter is running.
- Created arrays' tuple dimensions match their attribute matrix's tuple dimensions.
- Values that cannot be negative should be positive, and vice versa.
- The value that was input into the filter is in range. (i.e. An unsigned 8-bit integer array should be initialized only with values between 0-255).


<a name="execute">

### execute() ###

</a>

The **execute()** function runs when the user actually starts the pipeline.  This function contains the code that actually accomplishes the filter's intended objective.

<a name="getterfunctions">

### Getter Functions ###

</a>


The **getter functions** allow the filter to procure various values for other areas of DREAM.3D to use.  These functions are:

**getHumanLabel()**: The *Filter Human Label* (or the *Filter Name* in the image above) is the human-readable name of the filter.

**getGroupName()**: The *Filter Group Name* is the name of the group that the filter is categorized in.  In the Filter Library, filters can be found according to Filter Group Name.

**getSubGroupName()**: The *Filter Subgroup Name* is the subgroup that the filter is categorized in.  This is a secondary group within the filter's group.

**getFilterVersion()**: The *Filter Version* is the filter's unique version number.

**getBrandingString()**: The *Branding String* is a custom string that appears in the user interface to further label the filter.

**getCompiledLibraryName()**: The *Compiled Library Name* (or the *Compiled Plugin Name* in the image above) is the name of the plugin group that this filter is categorized in.

---
**Previous Page**: [Creating a Filter](http://dream3d.bluequartz.net/binaries/Help/DREAM3D/creating_a_filter.html)