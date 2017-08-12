#ifndef MPLPLOTWIDGETTEST_H
#define MPLPLOTWIDGETTEST_H

#include "cxxtest/TestSuite.h"

#include "MantidQtWidgets/MplCpp/MplPlotWidget.h"

using namespace MantidQt::Widgets::MplCpp;

class MplPlotWidgetTest : public CxxTest::TestSuite {
public:
  static MplPlotWidgetTest *createSuite() { return new MplPlotWidgetTest; }
  static void destroySuite(MplPlotWidgetTest *suite) { delete suite; }

  void test_Default_Construction_Yields_Single_Subplot() {
    MplPlotWidget mplplot;
    TSM_ASSERT_EQUALS("Default canvas should have a single subplot",
                      SubPlotSpec(1, 1), mplplot.canvas().getGeometry());
  }

  void test_Construction_With_SubPlot_Layout_Respects_It() {
    MplPlotWidget mplplot(231);
    TSM_ASSERT_EQUALS("Canvas should respect subplot layout request",
                      SubPlotSpec(2, 3), mplplot.canvas().getGeometry());
  }
};

#endif // MPLPLOTWIDGETTEST_H
