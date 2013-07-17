/*
 * Your License or Copyright Information can go here
 */

#include "TestFilters.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt0::Filt0() :
AbstractFilter(),
m_Float(6.6f),
m_Integer(15)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt0::~Filt0()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  /* Place all your option initialization code here */

   /*  For an Integer use this code*/
   {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Integer");
    option->setPropertyName("Integer");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
   /*  For a Floating point value use this code*/
   {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Float");
    option->setPropertyName("Float");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }


  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::readFilterParameters(AbstractFilterParametersReader* reader)
{
  float dummyFloat = 0.0f;
  int dummyInt = 0;
  setFloat( reader->readValue("Float", dummyFloat) );
  setInteger( reader->readValue("Integer", dummyInt) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Filt0::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
    writer->writeValue("Float", getFloat() );
    writer->writeValue("Integer", getInteger() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt0::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt1::Filt1() :
AbstractFilter(),
m_Float(6.6f),
m_Integer(15)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Filt1::~Filt1()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  /* Place all your option initialization code here */

   /*  For an Integer use this code*/
   {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Integer");
    option->setPropertyName("Integer");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
   /*  For a Floating point value use this code*/
   {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Float");
    option->setPropertyName("Float");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }


  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::readFilterParameters(AbstractFilterParametersReader* reader)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Filt1::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
    writer->writeValue("Float", getFloat() );
    writer->writeValue("Integer", getInteger() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Filt1::execute()
{
  int err = 0;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
