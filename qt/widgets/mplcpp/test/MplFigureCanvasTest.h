#ifndef MPLFIGURECANVASTEST_H
#define MPLFIGURECANVASTEST_H

#include "cxxtest/TestSuite.h"

#include "MantidQtWidgets/MplCpp/MplFigureCanvas.h"
#include "MantidQtWidgets/MplCpp/PythonErrors.h"

using namespace MantidQt::Widgets::MplCpp;

class MplFigureCanvasTest : public CxxTest::TestSuite {
public:
  static MplFigureCanvasTest *createSuite() { return new MplFigureCanvasTest; }
  static void destroySuite(MplFigureCanvasTest *suite) { delete suite; }

  //---------------------------------------------------------------------------
  // Success
  //---------------------------------------------------------------------------
  void test_Default_Construction_Yields_Single_Subplot() {
    MplFigureCanvas canvas;
    TSM_ASSERT_EQUALS("Default canvas should have a single subplot",
                      SubPlotSpec(1, 1), canvas.getGeometry());
    TSM_ASSERT_EQUALS("Default canvas should have 0 lines", 0, canvas.nlines());
  }

  void test_Construction_With_SubPlot_Layout_Respects_It() {
    MplFigureCanvas canvas(231);
    TSM_ASSERT_EQUALS("Canvas should respect subplot layout request",
                      SubPlotSpec(2, 3), canvas.getGeometry());
    TSM_ASSERT_EQUALS("Default canvas should have 0 lines", 0, canvas.nlines());
  }

  void test_Adding_A_Line_Increase_Line_Count_By_One() {
    MplFigureCanvas canvas;
    std::vector<double> data{1, 2, 3, 4, 5};
    canvas.plotLine(data, data, "r-");
    TSM_ASSERT_EQUALS("plotLine should increase line count by one", 1,
                      canvas.nlines());
  }

  void test_Removing_A_Line_Decreases_Line_Count_By_One() {
    MplFigureCanvas canvas;
    std::vector<double> data{1, 2, 3, 4, 5};
    canvas.plotLine(data, data, "r-");
    canvas.removeLine(0);
    TSM_ASSERT_EQUALS("removeLine should decrease line count by one", 0,
                      canvas.nlines());
  }

  //---------------------------------------------------------------------------
  // Failure
  //---------------------------------------------------------------------------

  void test_PlotLine_With_Different_Length_Arrays_Throws() {
    MplFigureCanvas canvas;
    std::vector<double> arr1{1, 2, 3}, arr2{1, 2, 3, 4};
    TSM_ASSERT_THROWS("plotLine should throw if len(x) < len(y)",
                      canvas.plotLine(arr1, arr2, "r-"), PythonError);
    TSM_ASSERT_THROWS("plotLine should throw if len(x) > len(y)",
                      canvas.plotLine(arr2, arr1, "r-"), PythonError);
  }
};

#endif // MPLFIGURECANVASTEST_H
