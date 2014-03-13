#include "MantidQtCustomDialogs/CatalogPublishDialog.h"

#include "MantidAPI/CatalogFactory.h"
#include "MantidAPI/CatalogManager.h"
#include "MantidAPI/ICatalog.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidKernel/ConfigService.h"
#include "MantidKernel/FacilityInfo.h"
#include "MantidQtAPI/AlgorithmInputHistory.h"
#include "MantidQtMantidWidgets/DataSelector.h"

#include <QDir>

namespace MantidQt
{
  namespace CustomDialogs
  {
    DECLARE_DIALOG(CatalogPublishDialog);

    /**
     * Default constructor.
     * @param parent :: Parent dialog.
     */
    CatalogPublishDialog::CatalogPublishDialog(QWidget *parent) : MantidQt::API::AlgorithmDialog(parent), m_uiForm() {}

    /// Initialise the layout
    void CatalogPublishDialog::initLayout()
    {
      m_uiForm.setupUi(this);
      this->setWindowTitle(m_algName);

      tie(m_uiForm.nameInCatalogTxt,"NameInCatalog");
      tie(m_uiForm.investigationNumberCb,"InvestigationNumber");
      tie(m_uiForm.descriptionInput,"DataFileDescription");

      // Assign the buttons with the inherited methods.
      connect(m_uiForm.runBtn,SIGNAL(clicked()),this,SLOT(accept()));
      connect(m_uiForm.cancelBtn,SIGNAL(clicked()),this,SLOT(reject()));
      connect(m_uiForm.helpBtn,SIGNAL(clicked()),this,SLOT(helpClicked()));
      // When the user selects a workspace, we want to be ready to publish it.
      connect(m_uiForm.dataSelector,SIGNAL(dataReady(const QString&)),this,SLOT(workspaceSelected(const QString&)));
      // When a file is chosen to be published, set the related "FileName" property of the algorithm.
      connect(m_uiForm.dataSelector,SIGNAL(filesFound()),this,SLOT(fileSelected()));

      // Populate "investigationNumberCb" with the investigation IDs that the user can publish to.
      populateUserInvestigations();

      // Get optional message here as we may set it if user has no investigations to publish to.
      m_uiForm.instructions->setText(getOptionalMessage());
    }

    /**
     * Populate the investigation number combo-box with investigations that the user can publish to.
     */
    void CatalogPublishDialog::populateUserInvestigations()
    {
      auto workspace = Mantid::API::WorkspaceFactory::Instance().createTable();

      // This again is a temporary measure to ensure publishing functionality will work with one catalog.
      auto session = Mantid::API::CatalogManager::Instance().getActiveSessions();
      if (!session.empty()) Mantid::API::CatalogManager::Instance().getCatalog(session.front()->getSessionId())->myData(workspace);

      // The user is not an investigator on any investigations and cannot publish
      // or they are not logged into the catalog then update the related message..
      if (workspace->rowCount() == 0)
      {
        setOptionalMessage("You cannot publish datafiles as you are not an investigator on any investigations or are not logged into the catalog.");
        // Disable the input fields and run button to prevent user from running algorithm.
        m_uiForm.scrollArea->setDisabled(true);
        m_uiForm.runBtn->setDisabled(true);
        return;
      }

      // Populate the form with investigations that the user can publish to.
      for (size_t row = 0; row < workspace->rowCount(); row++)
      {
        m_uiForm.investigationNumberCb->addItem(QString::fromStdString(workspace->cell<std::string>(row, 0)));
        // Add better tooltip for ease of use (much easier to recall the investigation if title and instrument are also provided).
        m_uiForm.investigationNumberCb->setItemData(static_cast<int>(row),
            QString::fromStdString("The title of the investigation is: \"" + workspace->cell<std::string>(row, 1) +
                                   "\".\nThe instrument of the investigation is: \"" + workspace->cell<std::string>(row, 2)) + "\".",
                                   Qt::ToolTipRole);
      }
    }

    /**
     * Obtain the name of the workspace selected, and set it to the algorithm's property.
     * @param wsName :: The name of the workspace to publish.
     */
    void CatalogPublishDialog::workspaceSelected(const QString& wsName)
    {
      // Prevents both a file and workspace being published at same time.
      storePropertyValue("FileName", "");
      setPropertyValue("FileName", true);
      // Set the workspace property to the one the user has selected to publish.
      storePropertyValue("InputWorkspace", wsName);
      setPropertyValue("InputWorkspace", true);
    }

    /**
     * Set the "FileName" property when a file is selected from the file browser.
     */
    void CatalogPublishDialog::fileSelected()
    {
      // Reset workspace property as the input is a file. This prevents both being selected.
      storePropertyValue("InputWorkspace", "");
      setPropertyValue("InputWorkspace", true);
      // Set the FileName property to the path that appears in the input field on the dialog.
      storePropertyValue("FileName", m_uiForm.dataSelector->getFullFilePath());
      setPropertyValue("FileName", true);
    }

    /**
     * Overridden to enable dataselector validators.
     */
    void CatalogPublishDialog::accept()
    {
      if (!m_uiForm.dataSelector->isValid())
      {
        if (m_uiForm.dataSelector->getFullFilePath().isEmpty())
        {
          QMessageBox::critical(this,"Error in catalog publishing.","No file specified.");
          return;
        }
        QMessageBox::critical(this,"Error in catalog publishing.",m_uiForm.dataSelector->getProblem());
        return;
      }
      AlgorithmDialog::accept();
    }
  }
}
