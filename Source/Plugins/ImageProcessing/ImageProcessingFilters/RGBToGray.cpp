/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "RGBToGray.h"

#include "ITKUtilities.h"

#include "itkUnaryFunctorImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RGBToGray::RGBToGray() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  m_ColorWeights.x = 0.2125;
  m_ColorWeights.y = 0.7154;
  m_ColorWeights.z = 0.0721;
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
  std::vector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Array to Process");
    parameter->setPropertyName("SelectedCellArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    options.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Color Weights");
    option->setPropertyName("ColorWeights");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("RGB");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Created Array Name");
    parameter->setPropertyName("NewCellArrayName");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    options.push_back(parameter);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setColorWeights( reader->readValue( "ColorWeights", getColorWeights() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RGBToGray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("ColorWeights", getColorWeights() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_SelectedCellArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "An array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  }
  else
  {
    m_RawImageDataArrayName=m_SelectedCellArrayName;
    GET_PREREQ_DATA(m, DREAM3D, CellData, RawImageData, ss, -300, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, voxels, 3)

    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, 0, voxels, 1)
    m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);

    if(0==m_ColorWeights.x+m_ColorWeights.y+m_ColorWeights.z)
    {
      setErrorCondition(-201);
      ss.str("");
      ss << "Sum of color weighting = 0.";
      addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

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
      //return static_cast<TOutput>( (A[0]*m_ColorWeights.x+A[1]*m_ColorWeights.y+A[2]*m_ColorWeights.z) );
      return static_cast<TOutput>( A[0]*weight_r+A[1]*weight_g+A[2]*weight_b );
      //return static_cast<TOutput>( A[0]*weights[0]+A[1]*weights[1]+A[2]*weights[2] );
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RGBToGray::execute()
{
  std::stringstream ss;
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
  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  //normalize color weights
  double mag = m_ColorWeights.x+m_ColorWeights.y+m_ColorWeights.z;
  m_ColorWeights.x=m_ColorWeights.x/mag;
  m_ColorWeights.y=m_ColorWeights.y/mag;
  m_ColorWeights.z=m_ColorWeights.z/mag;

  //wrap input array as image
  ImageProcessing::RGBDefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, reinterpret_cast<ImageProcessing::RGBDefaultPixelType*>(m_RawImageData));

  //set up conversion
  typedef itk::UnaryFunctorImageFilter< ImageProcessing::RGBDefaultImageType, ImageProcessing::DefaultImageType, Functor::Luminance<ImageProcessing::RGBDefaultPixelType, ImageProcessing::DefaultPixelType> > RGBToGrayType;
  RGBToGrayType::Pointer rgbToGray = RGBToGrayType::New();
  rgbToGray->GetFunctor().SetRWeight(m_ColorWeights.x);
  rgbToGray->GetFunctor().SetGWeight(m_ColorWeights.y);
  rgbToGray->GetFunctor().SetBWeight(m_ColorWeights.z);
  rgbToGray->SetInput(inputImage);

  //set output and execute
  ITKUtilities::SetITKOutput(rgbToGray->GetOutput(), m_ProcessedImageData, totalPoints);
  rgbToGray->Update();

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
