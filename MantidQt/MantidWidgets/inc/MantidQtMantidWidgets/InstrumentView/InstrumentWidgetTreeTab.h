#ifndef INSTRUMENTWIDGETTREETAB_H_
#define INSTRUMENTWIDGETTREETAB_H_

#include <WidgetDllOption.h>
#include "InstrumentWidgetTab.h"

#include <QModelIndex>

namespace MantidQt
{
	namespace MantidWidgets
	{
		class InstrumentTreeWidget;

		/**
			* Implements the instrument tree tab in InstrumentWidget
			*/
		class EXPORT_OPT_MANTIDQT_MANTIDWIDGETS InstrumentWidgetTreeTab : public InstrumentWidgetTab
		{
			Q_OBJECT
		public:
			explicit InstrumentWidgetTreeTab(InstrumentWidget *instrWidget);
			void initSurface();
			public slots:
			void selectComponentByName(const QString& name);
		private:
			void showEvent(QShowEvent *);
			/// Widget to display instrument tree
			InstrumentTreeWidget* m_instrumentTree;
		};
	}//MantidWidgets
}//MantidQt


#endif /*INSTRUMENTWIDGETTREETAB_H_*/
