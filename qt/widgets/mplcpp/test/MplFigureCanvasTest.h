#ifndef MPLPLOTWIDGETTEST_H
#define MPLPLOTWIDGETTEST_H

#include "cxxtest/TestSuite.h"

#include "MantidQtWidgets/MplCpp/MplFigureCanvas.h"

using namespace MantidQt::Widgets::MplCpp;

class MplFigureCanvasTestImpl : public QObject {
  Q_OBJECT
private slots:
};

class MplFigureCanvasTest : public CxxTest::TestSuite {
public:
  static MplFigureCanvasTest *createSuite() { return new MplFigureCanvasTest; }
  static void destroySuite(MplFigureCanvasTest *suite) { delete suite; }

  void test_Default_Construction_Yields_Single_Subplot() {
    MplFigureCanvas canvas;
    TSM_ASSERT_EQUALS("Default canvas should have a single subplot",
                      SubPlotSpec(1, 1), canvas.getGeometry());
  }

  void test_Construction_With_SubPlot_Layout_Respects_It() {
    MplFigureCanvas canvas(231);
    TSM_ASSERT_EQUALS("Canvas should respect subplot layout request",
                      SubPlotSpec(2, 3), canvas.getGeometry());
  }
};

#endif // MPLPLOTWIDGETTEST_H
