#ifndef FINDDIALOG_H_
#define FINDDIALOG_H_

//--------------------------------------------------
// Includes
//--------------------------------------------------
#include "MantidUiWidgetsCore/FindReplaceDialog.h"

/**
 * Specialisation of FindReplaceDialog that only
 * does finding
 */
class MANTIDUI_WIDGETS_CORE_PUBLIC FindDialog : public FindReplaceDialog {
  Q_OBJECT

public:
  FindDialog(ScriptEditor *editor, Qt::WindowFlags flags = 0);
};

#endif // FINDDIALOG_H_
