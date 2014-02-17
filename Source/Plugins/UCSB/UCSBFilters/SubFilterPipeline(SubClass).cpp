/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SubFilterPipeline.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubFilterPipeline::SubFilterPipeline() :
FilterPipeline()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SubFilterPipeline::~SubFilterPipeline()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::run()
{
  execute();
  FilterPipeline::pipelineFinished();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SubFilterPipeline::execute()
{
  int err = 0;


  VoxelDataContainer::Pointer dataContainer = getVoxelDataContainer();
  dataContainer->addObserver(static_cast<Observer*>(this));

  SurfaceMeshDataContainer::Pointer sm = getSurfaceMeshDataContainer();
  sm->addObserver(static_cast<Observer*>(this));

  SolidMeshDataContainer::Pointer solid = getSolidMeshDataContainer();
  solid->addObserver(static_cast<Observer*>(this));

  // Start looping through the Pipeline
  float progress = 0.0f;
  std::stringstream ss;

// Start a Benchmark Clock so we can keep track of each filter's execution time
  DEFINE_CLOCK;
  START_CLOCK;
  PipelineMessage progValue("", "", 0, PipelineMessage::StatusValue, -1);

  FilterPipeline::FilterContainerType& pipeline = getFilterContainer();

  for (FilterContainerType::iterator iter = pipeline.begin(); iter != pipeline.end(); ++iter)
  {
    progress = progress + 1.0f;
    progValue.setMessageType(PipelineMessage::StatusValue);
    progValue.setProgressValue(static_cast<int>( progress / (pipeline.size() + 1) * 100.0f ));
    sendPipelineMessage(progValue);
    //pipelineProgress(static_cast<int>( progress / (m_Pipeline.size() + 1) * 100.0f ));

    ss.str("");
    //ss << "[" << progress << "/" << m_Pipeline.size() << "] " << (*iter)->getHumanLabel() << " ";
    ss << "[" << progress << "/" << pipeline.size() << "] " << (*iter)->getHumanLabel() << " ";
  //  std::cout << ss.str() << std::endl;
    progValue.setMessageType(PipelineMessage::StatusMessage);
    progValue.setMessageText(ss.str());
    sendPipelineMessage(progValue);
    (*iter)->setMessagePrefix(ss.str());
    (*iter)->addObserver(static_cast<Observer*>(this));
    (*iter)->setVoxelDataContainer(dataContainer.get());
    (*iter)->setSurfaceMeshDataContainer(sm.get());
    (*iter)->setSolidMeshDataContainer(solid.get());
    setCurrentFilter(*iter);
    (*iter)->execute();
    (*iter)->removeObserver(static_cast<Observer*>(this));
    (*iter)->setVoxelDataContainer(NULL);
    (*iter)->setSurfaceMeshDataContainer(NULL);
    (*iter)->setSolidMeshDataContainer(NULL);
    err = (*iter)->getErrorCondition();
    if(err < 0)
    {
      setErrorCondition(err);
      sendPipelineMessages((*iter)->getPipelineMessages());
      progValue.setMessageType(PipelineMessage::Error);
      progValue.setProgressValue(100);
      sendPipelineMessage(progValue);
      pipelineFinished();
      return;
    }
    if (this->getCancel() == true)
    {
      break;
    }
    ss.str("");
     ss << (*iter)->getNameOfClass() << " Filter Complete";
    END_CLOCK(ss.str());
  }

  PipelineMessage completMessage("", "Pipeline Complete", 0, PipelineMessage::StatusMessage, -1);
  sendPipelineMessage(completMessage);
}
