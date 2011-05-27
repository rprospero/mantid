#include "vtkRebinningCutter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkAlgorithm.h"
#include "vtkPVClipDataSet.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkImplicitFunction.h"
#include "vtkPointData.h"
#include "vtkBox.h"

#include "MantidKernel/Exception.h"
#include "MantidMDAlgorithms/PlaneImplicitFunction.h"
#include "MantidMDAlgorithms/BoxImplicitFunction.h"
#include "MantidMDAlgorithms/NullImplicitFunction.h"
#include "MantidGeometry/MDGeometry/IMDDimensionFactory.h"
#include "MantidVatesAPI/EscalatingRebinningActionManager.h"
#include "MantidVatesAPI/RebinningCutterXMLDefinitions.h"
#include "MantidVatesAPI/vtkThresholdingUnstructuredGridFactory.h"
#include "MantidVatesAPI/vtkThresholdingHexahedronFactory.h"
#include "MantidVatesAPI/vtkThresholdingQuadFactory.h"
#include "MantidVatesAPI/vtkThresholdingLineFactory.h"
#include "MantidVatesAPI/IMDWorkspaceProxy.h"
#include "MantidVatesAPI/vtkProxyFactory.h"
#include "MantidVatesAPI/TimeToTimeStep.h"
#include "MantidVatesAPI/FilteringUpdateProgressAction.h"
#include "MantidVatesAPI/Common.h"
#include "MantidVatesAPI/vtkDataSetToGeometry.h" 
#include "MantidGeometry/MDGeometry/MDGeometryXMLParser.h"
#include "MantidGeometry/MDGeometry/MDGeometryXMLBuilder.h"

#include <boost/functional/hash.hpp>
#include <sstream>

/** Plugin for ParaView. Performs simultaneous rebinning and slicing of Mantid data.

 @author Owen Arnold, Tessella plc
 @date 14/03/2011

 Copyright &copy; 2010 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

 This file is part of Mantid.

 Mantid is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 Mantid is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
 Code Documentation is available at: <http://doxygen.mantidproject.org>
 */

vtkCxxRevisionMacro(vtkRebinningCutter, "$Revision: 1.0 $")
;
vtkStandardNewMacro(vtkRebinningCutter)
;

using namespace Mantid::VATES;

vtkRebinningCutter::vtkRebinningCutter() :
  m_presenter(),
  m_clipFunction(NULL),
  m_cachedVTKDataSet(NULL),
  m_clip(ApplyClipping),
  m_originalExtents(IgnoreOriginal),
  m_setup(Pending),
  m_timestep(0),
  m_thresholdMax(10000),
  m_thresholdMin(0),
  m_actionRequester(new Mantid::VATES::EscalatingRebinningActionManager())
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

vtkRebinningCutter::~vtkRebinningCutter()
{
}

std::string vtkRebinningCutter::createRedrawHash() const
{
  size_t seed = 1;
  using namespace boost;
  hash_combine(seed, m_thresholdMax);
  hash_combine(seed, m_thresholdMin);
  //TODO add other properties that should force redraw only when changed.
  std::stringstream sstream;
  sstream << seed;
  return sstream.str();
}

void vtkRebinningCutter::determineAnyCommonExecutionActions(const int timestep, BoxFunction_sptr box)
{
  //Handles some commong iteration actions that can only be determined at execution time.
  if ((timestep != m_timestep))
  {
    m_actionRequester->ask(RecalculateVisualDataSetOnly);
  }
  if (m_cachedRedrawArguments != createRedrawHash())
  {
    m_actionRequester->ask(RecalculateVisualDataSetOnly);
  }
  if(m_box.get() != NULL && *m_box != *box && IgnoreClipping != m_clip) //TODO: clean this up.
  {
     m_actionRequester->ask(RecalculateAll); //The clip function must have changed.
  }
}

int vtkRebinningCutter::RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  try
  {
  using namespace Mantid::Geometry;
  using namespace Mantid::MDDataObjects;
  using namespace Mantid::MDAlgorithms;
  using Mantid::VATES::Dimension_sptr;
  using Mantid::VATES::DimensionVec;

  //Setup is not complete until metadata has been correctly provided.
  if(SetupDone == m_setup)
  {
    vtkInformation * inputInf = inputVector[0]->GetInformationObject(0);
    vtkDataSet * inputDataset = vtkDataSet::SafeDownCast(inputInf->Get(vtkDataObject::DATA_OBJECT()));

    //Create the composite holder.
    CompositeImplicitFunction* compFunction = new CompositeImplicitFunction;
    vtkDataSet* clipDataSet = m_cachedVTKDataSet == NULL ? inputDataset : m_cachedVTKDataSet;
   
    BoxFunction_sptr box = constructBox(inputDataset); // Save the implicit function so that we may later determine if the extents have changed.
    compFunction->addFunction(box);
    
    // Construct reduction knowledge.
    m_presenter.constructReductionKnowledge(
      m_vecAppliedDimensions,
      m_appliedXDimension,
      m_appliedYDimension,
      m_appliedZDimension,
      m_appliedTDimension,
      compFunction,
      inputDataset);

    FilterUpdateProgressAction<vtkRebinningCutter> updatehandler(this);

    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(
      vtkDataObject::DATA_OBJECT()));

    //Actually perform rebinning or specified action.
    int timestep = 0;
    if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS()) && hasTDimension())
    {
      // usually only one actual step requested
      timestep = static_cast<int>(outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS())[0]);
    }
    
    determineAnyCommonExecutionActions(timestep, box);

    vtkUnstructuredGrid* outData;
    RebinningIterationAction action = m_actionRequester->action();
    Mantid::API::IMDWorkspace_sptr spRebinnedWs = m_presenter.applyRebinningAction(action, updatehandler);

    //Build a vtkDataSet
    int ndims = spRebinnedWs->getNumDims();
    vtkDataSetFactory_sptr spvtkDataSetFactory = createDataSetFactory(spRebinnedWs);
    outData = dynamic_cast<vtkUnstructuredGrid*> (m_presenter.createVisualDataSet(spvtkDataSetFactory));
    
    
    m_timestep = timestep; //Not settable directly via a setter.
    m_box = box;
    m_cachedRedrawArguments = createRedrawHash();
    m_actionRequester->reset(); //Restore default
    if(IgnoreClipping == m_clip)
    {
      m_cachedVTKDataSet =outData; 
    }
    output->ShallowCopy(outData);
  }
  return 1;
  }
  catch(std::exception& ex)
  {
    std::string what = ex.what();
  }
}

void vtkRebinningCutter::UpdateAlgorithmProgress(double progress)
{
  this->SetProgressText("Executing Mantid Rebinning Algorithm...");
  this->UpdateProgress(progress);
}

int vtkRebinningCutter::RequestInformation(vtkInformation* vtkNotUsed(request), vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  enum Status{Bad=0, Good=1};
  Status status=Good;
  if (Pending == m_setup)
  {
    using namespace Mantid::Geometry;
    using namespace Mantid::MDDataObjects;
    using namespace Mantid::MDAlgorithms;
    using Mantid::VATES::Dimension_sptr;
    using Mantid::VATES::DimensionVec;

    vtkInformation * inputInf = inputVector[0]->GetInformationObject(0);
    vtkDataSet * inputDataset = vtkDataSet::SafeDownCast(inputInf->Get(vtkDataObject::DATA_OBJECT()));

    bool bGoodInputProvided = canProcessInput(inputDataset);
    if(true == bGoodInputProvided)
    { 
      DimensionVec dimensionsVec(4);

      vtkDataSetToGeometry metaDataProcessor(inputDataset);
      metaDataProcessor.execute();
      dimensionsVec[0] = metaDataProcessor.getXDimension();
      dimensionsVec[1] = metaDataProcessor.getYDimension();
      dimensionsVec[2] = metaDataProcessor.getZDimension();
      dimensionsVec[3] = metaDataProcessor.getTDimension();
      m_appliedXDimension = metaDataProcessor.getXDimension();
      m_appliedYDimension = metaDataProcessor.getYDimension();
      m_appliedZDimension = metaDataProcessor.getZDimension();
      m_appliedTDimension = metaDataProcessor.getTDimension();

      // Construct reduction knowledge.
      m_presenter.constructReductionKnowledge(dimensionsVec, dimensionsVec[0], dimensionsVec[1],
        dimensionsVec[2], dimensionsVec[3], inputDataset);
      //First time round, rebinning has to occur.
      m_actionRequester->ask(RecalculateAll);
      m_setup = SetupDone;
    }
    else
    {
      vtkErrorMacro("Rebinning operations require Rebinning Metadata. Have you provided a rebinning source?");
      status = Bad;
    }
  }
  setTimeRange(outputVector);
  return status;
  
}

int vtkRebinningCutter::RequestUpdateExtent(vtkInformation* vtkNotUsed(info), vtkInformationVector** vtkNotUsed(inputVector),
    vtkInformationVector* vtkNotUsed(outputVector))
{
  return 1;
}
;

int vtkRebinningCutter::FillInputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

void vtkRebinningCutter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkRebinningCutter::SetApplyClip(int applyClip)
{
   Clipping temp = applyClip == 1 ? ApplyClipping : IgnoreClipping;
   if(temp != m_clip)
   {
     m_clip = temp;
     if(m_clip == ApplyClipping)
     {
       m_originalExtents = ApplyOriginal;
       m_actionRequester->ask(RecalculateAll);
     }
   }
}

void vtkRebinningCutter::SetClipFunction(vtkImplicitFunction * func)
{
  vtkBox* box = dynamic_cast<vtkBox*>(func);

  if (box != m_clipFunction)
  {
    this->Modified();
    this->m_clipFunction = box;
  }
}

void vtkRebinningCutter::SetMaxThreshold(double maxThreshold)
{
  if (maxThreshold != m_thresholdMax)
  {
    this->Modified();
    this->m_thresholdMax = maxThreshold;
  }
}

void vtkRebinningCutter::SetMinThreshold(double minThreshold)
{
  if (minThreshold != m_thresholdMin)
  {
    this->Modified();
    this->m_thresholdMin = minThreshold;
  }
}

void vtkRebinningCutter::formulateRequestUsingNBins(Mantid::VATES::Dimension_sptr newDim)
{
  using Mantid::VATES::Dimension_const_sptr;
  if(newDim.get() != NULL)
  {
    try
    {
      //Requests that the dimension is found in the workspace.
      Dimension_const_sptr wsDim = m_presenter.getDimensionFromWorkspace(newDim->getDimensionId());
      //
      if(newDim->getNBins() != wsDim->getNBins())
      {
        //The number of bins has changed. Rebinning cannot be avoided.
        m_actionRequester->ask(RecalculateAll);
      }
    }
    catch(Mantid::Kernel::Exception::NotFoundError&)
    {
      //This happens if the workspace is not available in the analysis data service. Hence the rebinning algorithm has not yet been run.
      m_actionRequester->ask(RecalculateAll);
    }
  }
}

void vtkRebinningCutter::SetAppliedGeometryXML(std::string appliedGeometryXML)
{
  if(SetupDone == m_setup)
  {
    //Generate an xml representation of the existing geometry.
    using namespace Mantid::Geometry;
    MDGeometryBuilderXML<StrictDimensionPolicy> xmlBuilder;
    xmlBuilder.addXDimension(m_appliedXDimension);
    xmlBuilder.addYDimension(m_appliedYDimension);
    xmlBuilder.addZDimension(m_appliedZDimension);
    xmlBuilder.addTDimension(m_appliedTDimension);
    VecIMDDimension_sptr::iterator it = m_vecAppliedDimensions.begin();
    for(; it < m_vecAppliedDimensions.end(); it++)
    {
      xmlBuilder.addOrdinaryDimension(*it);
    }
    const std::string existingGeometryXML = xmlBuilder.create();

    //When geometry xml is provided and that xml is different from the exising xml.
    if(!appliedGeometryXML.empty() && existingGeometryXML != appliedGeometryXML)
    {
      this->Modified();
      Mantid::Geometry::MDGeometryXMLParser xmlParser(appliedGeometryXML);
      xmlParser.execute();
      if(xmlParser.getNonIntegratedDimensions().size() == 4)
      {
        m_actionRequester->ask(RecalculateVisualDataSetOnly); //will therefore use proxy.
      }
      else
      {
        m_actionRequester->ask(RecalculateAll);
      }

      Mantid::VATES::Dimension_sptr temp = xmlParser.getXDimension();
      formulateRequestUsingNBins(temp);
      this->m_appliedXDimension = temp;

      temp = xmlParser.getYDimension();
      formulateRequestUsingNBins(temp);
      this->m_appliedYDimension = temp;

      temp = xmlParser.getZDimension();
      formulateRequestUsingNBins(temp);
      this->m_appliedZDimension = temp;

      temp = xmlParser.getTDimension();
      formulateRequestUsingNBins(temp);
      this->m_appliedTDimension = temp;

      this->m_vecAppliedDimensions = xmlParser.getAllDimensions();
    }
  }
}

const char* vtkRebinningCutter::GetInputGeometryXML()
{
  try
  {
    return this->m_presenter.getWorkspaceGeometry().c_str();
  }
  catch(std::runtime_error&)
  {
    return "";
  }
}

unsigned long vtkRebinningCutter::GetMTime()
{
  unsigned long mTime = this->Superclass::GetMTime();
  unsigned long time;

  if (this->m_clipFunction != NULL)
  {
    
    time = this->m_clipFunction->GetMTime();
    if(time > mTime)
    {
      mTime = time;
    }
  }

  return mTime;
}



BoxFunction_sptr vtkRebinningCutter::constructBox(vtkDataSet* inputDataset) const
{
  using namespace Mantid::MDAlgorithms;

  double originX, originY, originZ, width, height, depth;
  if(ApplyClipping == m_clip)
  {
    vtkBox* box = m_clipFunction; //Alias
    //To get the box bounds, we actually need to evaluate the box function. There is not this restriction on planes.
    vtkPVClipDataSet * cutter = vtkPVClipDataSet::New();
    cutter->SetInput(inputDataset);
    cutter->SetClipFunction(box);
    cutter->SetInsideOut(true);
    cutter->Update();
    vtkDataSet* cutterOutput = cutter->GetOutput();
    //Now we can get the bounds.
    double* bounds = cutterOutput->GetBounds();

    originX = (bounds[1] + bounds[0]) / 2;
    originY = (bounds[3] + bounds[2]) / 2;
    originZ = (bounds[5] + bounds[4]) / 2;
    width = sqrt(pow(bounds[1] - bounds[0], 2));
    height = sqrt(pow(bounds[3] - bounds[2], 2));
    depth = sqrt(pow(bounds[5] - bounds[4], 2));
  }
  else
  {
    vtkDataSetToGeometry metaDataProcessor(inputDataset);
    metaDataProcessor.execute();

    originX = (metaDataProcessor.getXDimension()->getMaximum() + metaDataProcessor.getXDimension()->getMinimum()) / 2;
    originY = (metaDataProcessor.getYDimension()->getMaximum() + metaDataProcessor.getYDimension()->getMinimum()) / 2;
    originZ = (metaDataProcessor.getZDimension()->getMaximum() + metaDataProcessor.getZDimension()->getMinimum()) / 2;
    width = metaDataProcessor.getXDimension()->getMaximum() - metaDataProcessor.getXDimension()->getMinimum();
    height = metaDataProcessor.getYDimension()->getMaximum() - metaDataProcessor.getYDimension()->getMinimum();
    depth = metaDataProcessor.getZDimension()->getMaximum() - metaDataProcessor.getZDimension()->getMinimum();
  } 
  //Create domain parameters.
  OriginParameter originParam = OriginParameter(originX, originY, originZ);
  WidthParameter widthParam = WidthParameter(width);
  HeightParameter heightParam = HeightParameter(height);
  DepthParameter depthParam = DepthParameter(depth);

  //Create the box. This is specific to this type of presenter and this type of filter. Other rebinning filters may use planes etc.
  BoxImplicitFunction* boxFunction = new BoxImplicitFunction(widthParam, heightParam, depthParam,
    originParam);

  return BoxFunction_sptr(boxFunction);
}

vtkDataSetFactory_sptr vtkRebinningCutter::createDataSetFactory(
    Mantid::API::IMDWorkspace_sptr spRebinnedWs) const
{
  if((m_actionRequester->action() == RecalculateVisualDataSetOnly) && hasXDimension() && hasYDimension() && hasZDimension() && hasTDimension())
  {
    //This route rebinds the underlying image in such a way that dimension swapping can
    //be achieved very rapidly.
    return createQuickChangeDataSetFactory(spRebinnedWs);
  }
  else
  {
    //This route regenerates the underlying image.
    return createQuickRenderDataSetFactory(spRebinnedWs);
  }
}

vtkDataSetFactory_sptr vtkRebinningCutter::createQuickChangeDataSetFactory(
    Mantid::API::IMDWorkspace_sptr spRebinnedWs) const
{
  //Get the time dimension
  boost::shared_ptr<const Mantid::Geometry::IMDDimension> timeDimension = spRebinnedWs->getTDimension();

  Mantid::API::IMDWorkspace_sptr workspaceProxy = IMDWorkspaceProxy::New
      (spRebinnedWs,
          m_appliedXDimension,
          m_appliedYDimension,
          m_appliedZDimension,
          m_appliedTDimension);


  //Create a factory for generating a thresholding unstructured grid.
  vtkDataSetFactory* pvtkDataSetFactory = new Mantid::VATES::vtkThresholdingUnstructuredGridFactory<TimeToTimeStep>
  (XMLDefinitions::signalName(), m_timestep, m_thresholdMin, m_thresholdMax);

  pvtkDataSetFactory->initialize(workspaceProxy);

  //Return the generated factory.
  return vtkDataSetFactory_sptr(pvtkDataSetFactory);
}

vtkDataSetFactory_sptr vtkRebinningCutter::createQuickRenderDataSetFactory(
    Mantid::API::IMDWorkspace_sptr spRebinnedWs) const
{
  std::string scalarName = XMLDefinitions::signalName();
  vtkThresholdingLineFactory* vtkGridFactory = new vtkThresholdingLineFactory(scalarName, m_thresholdMin, m_thresholdMax);
  vtkThresholdingQuadFactory* p_2dSuccessorFactory = new vtkThresholdingQuadFactory(scalarName, m_thresholdMin, m_thresholdMax);
  vtkThresholdingHexahedronFactory* p_3dSuccessorFactory = new vtkThresholdingHexahedronFactory(scalarName, m_thresholdMin, m_thresholdMax);
  vtkThresholdingUnstructuredGridFactory<TimeToTimeStep>* p_4dSuccessorFactory = new vtkThresholdingUnstructuredGridFactory<TimeToTimeStep>(scalarName, m_timestep, m_thresholdMin, m_thresholdMax);
  vtkGridFactory->SetSuccessor(p_2dSuccessorFactory);
  p_2dSuccessorFactory->SetSuccessor(p_3dSuccessorFactory);
  p_3dSuccessorFactory->SetSuccessor(p_4dSuccessorFactory);
  
  vtkGridFactory->initialize(spRebinnedWs);

  //Return the generated factory.
  return vtkDataSetFactory_sptr(vtkGridFactory);
}

void vtkRebinningCutter::setTimeRange(vtkInformationVector* outputVector)
{
  if(SetupDone == m_setup)
  {
    if(hasTDimension())
    {
      double min = m_appliedTDimension->getMinimum();
      double max = m_appliedTDimension->getMaximum();
      unsigned int nBins = static_cast<int>( m_appliedTDimension->getNBins() );
      double increment = (max - min) / nBins;
      std::vector<double> timeStepValues(nBins);
      for (unsigned int i = 0; i < nBins; i++)
      {
        timeStepValues[i] = min + (i * increment);
      }
      vtkInformation *outInfo = outputVector->GetInformationObject(0);
      outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &timeStepValues[0],
        static_cast<int> (timeStepValues.size()));
      double timeRange[2];
      timeRange[0] = timeStepValues.front();
      timeRange[1] = timeStepValues.back();

      outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
    }
  }
}

