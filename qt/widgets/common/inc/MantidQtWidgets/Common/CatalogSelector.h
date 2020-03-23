// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//   NScD Oak Ridge National Laboratory, European Spallation Source,
//   Institut Laue - Langevin & CSNS, Institute of High Energy Physics, CAS
// SPDX - License - Identifier: GPL - 3.0 +
#pragma once

#include "DllOption.h"
#include "ui_CatalogSelector.h"

namespace MantidQt {
namespace MantidWidgets {
class EXPORT_OPT_MANTIDQT_COMMON CatalogSelector : public QWidget {
  Q_OBJECT

public:
  /// Default constructor
  CatalogSelector(QWidget *parent = nullptr);
  /// Obtain the session information for the facilities selected.
  std::vector<std::string> getSelectedCatalogSessions();
  /// Populate the ListWidget with the facilities of the catalogs the user is
  /// logged in to.
  void populateFacilitySelection();

private:
  /// Initialise the layout
  virtual void initLayout();

private slots:
  /// Checks the checkbox of the list item selected.
  void checkSelectedFacility(QListWidgetItem *item);

protected:
  /// The form generated by QT Designer.
  Ui::CatalogSelector m_uiForm;
};
} // namespace MantidWidgets
} // namespace MantidQt
