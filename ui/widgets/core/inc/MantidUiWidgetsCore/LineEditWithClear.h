/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef LINEEDITWITHCLEAR_H
#define LINEEDITWITHCLEAR_H

#include <QLineEdit>
#include "MantidKernel/System.h"
#include "MantidUiWidgetsCore/DllOption.h"

class QToolButton;

namespace MantidQt {
namespace MantidWidgets {

class MANTIDUI_WIDGETS_CORE_PUBLIC LineEditWithClear : public QLineEdit {
  Q_OBJECT

public:
  LineEditWithClear(QWidget *parent = 0);

protected:
  void resizeEvent(QResizeEvent *) override;

private slots:
  void updateCloseButton(const QString &text);

private:
  QToolButton *clearButton;
};
}
}

#endif // LINEEDITWITHCLEAR_H
