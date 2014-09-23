#ifndef MANTIDQTCUSTOMINTERFACES_INDIRECTDIFFRACTIONREDUCTION_H_
#define MANTIDQTCUSTOMINTERFACES_INDIRECTDIFFRACTIONREDUCTION_H_

//----------------------
// Includes
//----------------------
#include "ui_IndirectDiffractionReduction.h"

#include "MantidQtAPI/BatchAlgorithmRunner.h"
#include "MantidQtAPI/UserSubWindow.h"

namespace MantidQt
{
namespace CustomInterfaces
{
class IndirectDiffractionReduction : public MantidQt::API::UserSubWindow
{
  Q_OBJECT

public:
  /// The name of the interface as registered into the factory
  static std::string name() { return "Diffraction"; }
  // This interface's categories.
  static QString categoryInfo() { return "Indirect"; }

public:
  /// Default Constructor
  IndirectDiffractionReduction(QWidget *parent = 0);
  ~IndirectDiffractionReduction();

public slots:
  void demonRun();
  void instrumentSelected(int);
  void reflectionSelected(int);
  void openDirectoryDialog();
  void help();

private:
  virtual void initLayout();
  void initLocalPython();

  void loadSettings();
  void saveSettings();

  bool validateDemon();

  Mantid::API::MatrixWorkspace_sptr loadInstrument(std::string instrumentName,
      std::string reflection = "");

  void runGenericReduction(QString instName, QString mode);
  void runOSIRISdiffonlyReduction();

private:
  Ui::IndirectDiffractionReduction m_uiForm;  /// The form generated using Qt Designer
  QIntValidator * m_valInt;
  QDoubleValidator * m_valDbl;
  QString m_settingsGroup;                    /// The settings group
  MantidQt::API::BatchAlgorithmRunner *m_batchAlgoRunner;

};

}
}

#endif //MANTIDQTCUSTOMINTERFACES_INDIRECTDIFFRACTIONREDUCTION_H_
