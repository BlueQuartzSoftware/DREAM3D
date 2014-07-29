/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "RGBToGray.h"

#include <string>

//thresholding filter
#include "itkUnaryFunctorImageFilter.h"

// ImageProcessing Plugin
#include "ITKUtilities.h"

namespace Functor
{
  template< class TInput, class TOutput> class Luminance
  {
  public:
    Luminance() {};
    ~Luminance() {};

    bool operator!=( const Luminance & ) const
    {
      return false;
    }
    bool operator==( const Luminance & other ) const
    {
      return !(*this != other);
    }

    inline TOutput operator()(const TInput & A) const
    {
      return static_cast<TOutput>( A[0]*weight_r+A[1]*weight_g+A[2]*weight_b );
    }

    void SetRWeight(double r)
    {
      weight_r=r;
    }
    void SetGWeight(double g)
    {
      weight_g=g;
    }
    void SetBWeight(double b)
    {
      weight_b=b;
    }

  private:
    double weight_r;
    double weight_g;
    double weight_b;
  };
}

/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename T>
class RGBToGrayPrivate
{
  public:
    typedef DataArray<T> DataArrayType;

    RGBToGrayPrivate() {}
    virtual ~RGBToGrayPrivate() {}

    // -----------------------------------------------------------------------------
    // Determine if this is the proper type of an array to downcast from the IDataArray
    // -----------------------------------------------------------------------------
    bool operator()(IDataArray::Pointer p)
    {
      return (boost::dynamic_pointer_cast<DataArrayType>(p).get() != NULL);
    }

    // -----------------------------------------------------------------------------
    // This is the actual templated algorithm
    // -----------------------------------------------------------------------------
    void static Execute(RGBToGray* filter, IDataArray::Pointer inputIDataArray, IDataArray::Pointer outputIDataArray, FloatVec3_t weights, VolumeDataContainer* m, QString attrMatName)
    {
      typename DataArrayType::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(inputIDataArray);
      typename DataArrayType::Pointer outputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(outputIDataArray);



      // Get the Raw Pointer to the Allocated Memory region (array) for the input and output arrays
      T* inputData = static_cast<T*>(inputDataPtr->getPointer(0));
      T* outputData = static_cast<T*>(outputDataPtr->getPointer(0));

      size_t numVoxels = inputDataPtr->getNumberOfTuples();

      //set weighting
      double mag = weights.x + weights.y + weights.z;

      // Define all the typedefs that are needed
      typedef ITKUtilities<T>                                           ITKUtilitiesType;
      typedef typename ITKUtilitiesType::RGBImageType                   RGBImageType;
      typedef typename RGBImageType::Pointer                            RGBImagePointerType;
      typedef typename RGBImageType::PixelType                          RGBImagePixelType;
      typedef typename ITKUtilitiesType::ScalarImageType                ScalarImageType;
      typedef typename ScalarImageType::PixelType                       ScalarImagePixelType;
      //define Fucntor Typedef
      typedef Functor::Luminance<RGBImagePixelType, ScalarImagePixelType>                       LuminanceFunctorType;
      //define filters typedef
      typedef itk::UnaryFunctorImageFilter<RGBImageType, ScalarImageType, LuminanceFunctorType> RGBToGrayType;


      //wrap input as itk image
      RGBImagePointerType inputImage
                        = ITKUtilitiesType::template Dream3DtoITKTemplate<RGBImageType>(m, attrMatName, inputData);

      //convert to gray
      typename RGBToGrayType::Pointer itkFilter = RGBToGrayType::New();
      itkFilter->GetFunctor().SetRWeight(weights.x/mag);
      itkFilter->GetFunctor().SetGWeight(weights.y/mag);
      itkFilter->GetFunctor().SetBWeight(weights.z/mag);
      itkFilter->SetInput(inputImage);
      itkFilter->GetOutput()->GetPixelContainer()->SetImportPointer(outputData, numVoxels, false);
      try
      {
        itkFilter->Update();
      }
      catch( itk::ExceptionObject & err )
      {
        filter->setErrorCondition(-5);
        QString ss = QObject::tr("Failed to convert image. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }

    }
  private:
    RGBToGrayPrivate(const RGBToGrayPrivate&); // Copy Constructor Not Implemented
    void operator=(const RGBToGrayPrivate&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RGBToGray::RGBToGray() :
  AbstractFilter(),
  m_SelectedCellArrayArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
{
  m_ColorWeights.x = 0.2125f;
  m_ColorWeights.y = 0.7154f;
  m_ColorWeights.z = 0.0721f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RGBToGray::~RGBToGray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Color Weighting", "ColorWeights", FilterParameterWidgetType::FloatVec3Widget, getColorWeights(), false));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayArrayPath( reader->readDataArrayPath( "SelectedCellArrayArrayPath", getSelectedCellArrayArrayPath() ) );
  setColorWeights( reader->readFloatVec3("ColorWeights", getColorWeights() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RGBToGray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ColorWeights)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  //check for required arrays
  QVector<size_t> dims(1, 3);
  TEMPLATE_GET_PREREQ_ARRAY(SelectedCellArray, getSelectedCellArrayArrayPath(), dims)

  //configured created name / location
  tempPath.update(getSelectedCellArrayArrayPath().getDataContainerName(), getSelectedCellArrayArrayPath().getAttributeMatrixName(), getNewCellArrayName() );

  //get type
  QString typeName = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayArrayPath().getDataContainerName())->getAttributeMatrix(getSelectedCellArrayArrayPath().getAttributeMatrixName())->getAttributeArray(getSelectedCellArrayArrayPath().getDataArrayName())->getTypeAsString();;
  int type = TemplateUtilities::getTypeFromTypeName(typeName);

  //create new array of same type
  dims[0]=1;
  TEMPLATE_CREATE_NONPREREQ_ARRAY(NewCellArray, tempPath, dims, type);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::execute()
{
  QString ss;
  dataCheck();
  if(getErrorCondition() < 0)
  {
    setErrorCondition(-10000);
    ss = QObject::tr("DataCheck did not pass during execute");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //get volume container
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayArrayPath().getAttributeMatrixName();

  //get input and output data
  IDataArray::Pointer inputData = m_SelectedCellArrayPtr.lock();
  IDataArray::Pointer outputData = m_NewCellArrayPtr.lock();

  //execute type dependant portion using a Private Implementation that takes care of figuring out if
  // we can work on the correct type and actually handling the algorithm execution. We pass in "this" so
  // that the private implementation can get access to the current object to pass up status notifications,
  // progress or handle "cancel" if needed.
  if(RGBToGrayPrivate<int8_t>()(inputData))
  {
    RGBToGrayPrivate<int8_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<uint8_t>()(inputData) )
  {
    RGBToGrayPrivate<uint8_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<int16_t>()(inputData) )
  {
    RGBToGrayPrivate<int16_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<uint16_t>()(inputData) )
  {
    RGBToGrayPrivate<uint16_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<int32_t>()(inputData) )
  {
    RGBToGrayPrivate<int32_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<uint32_t>()(inputData) )
  {
    RGBToGrayPrivate<uint32_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<int64_t>()(inputData) )
  {
    RGBToGrayPrivate<int64_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<uint64_t>()(inputData) )
  {
    RGBToGrayPrivate<uint64_t>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<float>()(inputData) )
  {
    RGBToGrayPrivate<float>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else if(RGBToGrayPrivate<double>()(inputData) )
  {
    RGBToGrayPrivate<double>::Execute(this, inputData, outputData, getColorWeights(), m, attrMatName);
  }
  else
  {
    setErrorCondition(-10001);
    ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //array name changing/cleanup
  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_SelectedCellArrayArrayPath.getAttributeMatrixName());
  attrMat->addAttributeArray(getNewCellArrayName(), outputData);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RGBToGray::newFilterInstance(bool copyFilterParameters)
{
  RGBToGray::Pointer filter = RGBToGray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RGBToGray::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RGBToGray::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RGBToGray::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RGBToGray::getHumanLabel()
{return "Convert RGB to Grayscale";}

