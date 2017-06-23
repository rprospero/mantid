#ifndef INSTRUMENTWIDGETTYPES_H_
#define INSTRUMENTWIDGETTYPES_H_

#include <MantidUiWidgetsCore/DllOption.h>

namespace MantidQt {
namespace MantidWidgets {
class MANTIDUI_WIDGETS_CORE_PUBLIC InstrumentWidgetTypes {

public:
  enum SurfaceType {
    FULL3D = 0,
    CYLINDRICAL_X,
    CYLINDRICAL_Y,
    CYLINDRICAL_Z,
    SPHERICAL_X,
    SPHERICAL_Y,
    SPHERICAL_Z,
    SIDE_BY_SIDE,
    RENDERMODE_SIZE
  };
};
} // MantidWidgets
} // MantidQt

#endif /*INSTRUMENTWIDGETTYPES_H_*/
