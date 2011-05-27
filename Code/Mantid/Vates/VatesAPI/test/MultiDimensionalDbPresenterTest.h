#ifndef MULTIDIMENSIONAL_DB_PRESENTER_TEST_H_
#define MULTIDIMENSIONAL_DB_PRESENTER_TEST_H_

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cxxtest/TestSuite.h>
#include <vtkPointData.h>
#include "MantidVatesAPI/TimeStepToTimeStep.h"
#include "MantidVatesAPI/vtkStructuredGridFactory.h"
#include "MantidVatesAPI/MultiDimensionalDbPresenter.h"
#include "MantidMDAlgorithms/Load_MDWorkspace.h"
#include "MantidKernel/System.h"

using namespace Mantid::VATES;

class MultiDimensionalDbPresenterTest : public CxxTest::TestSuite
{

private:

  static std::string getTestFileName(){return "test_horace_reader.sqw";}
  
  // Helper type. Facilitates testing without AnalysisDataService.
  class ExposedExecutor : public Mantid::VATES::MultiDimensionalDbPresenter
  {
  protected:
    virtual void extractWorkspaceImplementation(const std::string& wsId)
    {
      UNUSED_ARG(wsId);
      //Do nothing!.
    }
  };

  /// The VatesAPI domain type can handle any algorithm. Mock type to verify this.
  class MockDataHandlingAlgorithm : public Mantid::API::Algorithm
  {
  public:
    MOCK_CONST_METHOD0(isInitialized, bool());
    MOCK_CONST_METHOD0(name,const std::string());
    MOCK_CONST_METHOD0(version, int());
    MOCK_METHOD0(init, void());
    MOCK_METHOD0(exec, void());
  };

public:

void testExecution()
{
  MockDataHandlingAlgorithm algorithm;
  EXPECT_CALL(algorithm, name()).WillRepeatedly(testing::Return("MockDataHandlingAlgorithm"));
  EXPECT_CALL(algorithm, version()).WillRepeatedly(testing::Return(1));
  EXPECT_CALL(algorithm, isInitialized()).WillRepeatedly(testing::Return(true));
  EXPECT_CALL(algorithm, exec()).Times(1);
  ExposedExecutor presenter;
  presenter.execute(algorithm, "");
  TSM_ASSERT("Algorithm should have been executed.", testing::Mock::VerifyAndClearExpectations(&algorithm));
}

void testNotInitalizedThrowsOnExecution()
{
  MockDataHandlingAlgorithm algorithm;
  EXPECT_CALL(algorithm, name()).WillRepeatedly(testing::Return("MockDataHandlingAlgorithm"));
  EXPECT_CALL(algorithm, version()).WillRepeatedly(testing::Return(1));
  EXPECT_CALL(algorithm, isInitialized()).Times(1).WillOnce(testing::Return(false));
  EXPECT_CALL(algorithm, exec()).Times(0);
  ExposedExecutor presenter;
  TSM_ASSERT_THROWS("Should have thrown since algorithm is not passing isInitialized() test.", presenter.execute(algorithm, ""), std::invalid_argument);
}

//Simple schenario testing end-to-end working of this presenter.
void testConstruction()
{
  Mantid::MDAlgorithms::Load_MDWorkspace wsLoaderAlg;
  wsLoaderAlg.initialize();
  std::string wsId = "InputMDWs";
  wsLoaderAlg.setPropertyValue("inFilename", getTestFileName());
  wsLoaderAlg.setPropertyValue("MDWorkspace",wsId);

  MultiDimensionalDbPresenter mdPresenter;
  mdPresenter.execute(wsLoaderAlg, wsId);

  vtkStructuredGridFactory<TimeStepToTimeStep> vtkGridFactory("signal", 1);
  vtkDataArray* data = mdPresenter.getScalarDataFromTimeBin(vtkGridFactory);
  RebinningKnowledgeSerializer serializer;
  vtkDataSet* visData = mdPresenter.getMesh(serializer, vtkGridFactory);
  TSM_ASSERT_EQUALS("Incorrect number of scalar signal points.", 18, data->GetSize());
  TSM_ASSERT_EQUALS("Incorrect number of visualisation vtkPoints generated", 48, visData->GetNumberOfPoints());
  TSM_ASSERT_EQUALS("Incorrect number of timesteps returned", 2, mdPresenter.getNumberOfTimesteps());
  data->Delete();
  visData->Delete();
}

void testGetCycles()
{
  Mantid::MDAlgorithms::Load_MDWorkspace wsLoaderAlg;
  wsLoaderAlg.initialize();
  std::string wsId = "InputMDWs";
  wsLoaderAlg.setPropertyValue("inFilename", getTestFileName());
  wsLoaderAlg.setPropertyValue("MDWorkspace",wsId);

  MultiDimensionalDbPresenter mdPresenter;
  mdPresenter.execute(wsLoaderAlg, wsId);
  std::vector<int> vecCycles = mdPresenter.getCycles();
  TSM_ASSERT_EQUALS("Wrong number of cycles in cycles collection.", vecCycles.size(), mdPresenter.getNumberOfTimesteps());
}

void testGetTimesteps()
{
  Mantid::MDAlgorithms::Load_MDWorkspace wsLoaderAlg;
  wsLoaderAlg.initialize();
  std::string wsId = "InputMDWs";
  wsLoaderAlg.setPropertyValue("inFilename", getTestFileName());
  wsLoaderAlg.setPropertyValue("MDWorkspace",wsId);

  MultiDimensionalDbPresenter mdPresenter;
  mdPresenter.execute(wsLoaderAlg, wsId);
  std::vector<double> vecTimes = mdPresenter.getTimesteps();
  TSM_ASSERT_EQUALS("Wrong number of times in times collection.", vecTimes.size(), mdPresenter.getNumberOfTimesteps());
}

void testGetScalarDataThrows()
{
  MultiDimensionalDbPresenter mdPresenter;

  //No execution call. Test that type cannot be used improperly.
  vtkStructuredGridFactory<TimeStepToTimeStep> vtkGridFactory("signal", 1);
  TSM_ASSERT_THROWS("Accessing scalar data without first calling execute should not be possible", mdPresenter.getScalarDataFromTimeBin(vtkGridFactory), std::runtime_error);
}

void testGetMeshThrows()
{
  MultiDimensionalDbPresenter mdPresenter;

  //No execution call. Test that type cannot be used improperly.
  RebinningKnowledgeSerializer serializer;
  vtkStructuredGridFactory<TimeStepToTimeStep> vtkGridFactory("signal", 1);
  TSM_ASSERT_THROWS("Accessing mesh data without first calling execute should not be possible", mdPresenter.getMesh(serializer, vtkGridFactory), std::runtime_error);
}

void testGetNumberOfTimestepsThrows()
{
  MultiDimensionalDbPresenter mdPresenter;

  //No execution call. Test that type cannot be used improperly.

  TSM_ASSERT_THROWS("Accessing timestep number data without first calling execute should not be possible", mdPresenter.getNumberOfTimesteps(), std::runtime_error);
}

void testGetCylesThrows()
{
  MultiDimensionalDbPresenter mdPresenter;

  //No execution call. Test that type cannot be used improperly.

  TSM_ASSERT_THROWS("Accessing cycles data without first calling execute should not be possible", mdPresenter.getCycles(), std::runtime_error);

}

void testGetTimestepsThrows()
{

  MultiDimensionalDbPresenter mdPresenter;

  //No execution call. Test that type cannot be used improperly.

  TSM_ASSERT_THROWS("Accessing timestep data without first calling execute should not be possible", mdPresenter.getTimesteps(), std::runtime_error);
}


void testExecuteThrows()
{
  Mantid::MDAlgorithms::Load_MDWorkspace wsLoaderAlg;
  std::string wsId = "InputMDWs";
  //Note that the algorithm has not been initialized here.

  MultiDimensionalDbPresenter mdPresenter;
  
  TSM_ASSERT_THROWS("Cannot read using a data loading algorithm that has not been initialized.", mdPresenter.execute(wsLoaderAlg, wsId), std::invalid_argument);
}



};

#endif
