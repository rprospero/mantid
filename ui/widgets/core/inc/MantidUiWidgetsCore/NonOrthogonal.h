#ifndef MANTID_MANTIDQT_API_NON_ORTHOGONAL_H_
#define MANTID_MANTIDQT_API_NON_ORTHOGONAL_H_

#include "MantidUiWidgetsCore/DllOption.h"
#include "MantidAPI/IMDWorkspace.h"
#include "MantidKernel/Matrix.h"

namespace MantidQt {
namespace API {

enum class DimensionSelection { H, K, L };

void MANTIDUI_WIDGETS_CORE_PUBLIC
provideSkewMatrix(Mantid::Kernel::DblMatrix &skewMatrix,
                  Mantid::API::IMDWorkspace_const_sptr workspace);

bool MANTIDUI_WIDGETS_CORE_PUBLIC
requiresSkewMatrix(Mantid::API::IMDWorkspace_const_sptr workspace);

bool MANTIDUI_WIDGETS_CORE_PUBLIC
isHKLDimensions(Mantid::API::IMDWorkspace_const_sptr workspace, size_t dimX,
                size_t dimY);

size_t MANTIDUI_WIDGETS_CORE_PUBLIC
getMissingHKLDimensionIndex(Mantid::API::IMDWorkspace_const_sptr workspace,
                            size_t dimX, size_t dimY);

void MANTIDUI_WIDGETS_CORE_PUBLIC
transformFromDoubleToCoordT(Mantid::Kernel::DblMatrix &skewMatrix,
                            Mantid::coord_t skewMatrixCoord[9]);

void MANTIDUI_WIDGETS_CORE_PUBLIC
transformLookpointToWorkspaceCoord(Mantid::coord_t *lookPoint,
                                   const Mantid::coord_t skewMatrix[9],
                                   const size_t &dimX, const size_t &dimY,
                                   const size_t &dimSlice);

template <typename T>
void transformLookpointToWorkspaceCoordGeneric(
    T &lookPoint, const Mantid::coord_t skewMatrix[9], const size_t &dimX,
    const size_t &dimY, const size_t &dimSlice) {
  auto sliceDimResult =
      (lookPoint[dimSlice] - skewMatrix[3 * dimSlice + dimX] * lookPoint[dimX] -
       skewMatrix[3 * dimSlice + dimY] * lookPoint[dimY]) /
      skewMatrix[3 * dimSlice + dimSlice];

  auto OrigDimSliceValue = lookPoint[dimSlice];
  lookPoint[dimSlice] = sliceDimResult;

  auto v1 = lookPoint[0];
  auto v2 = lookPoint[1];
  auto v3 = lookPoint[2];

  lookPoint[dimX] = v1 * skewMatrix[0 + 3 * dimX] +
                    v2 * skewMatrix[1 + 3 * dimX] +
                    v3 * skewMatrix[2 + 3 * dimX];
  lookPoint[dimY] = v1 * skewMatrix[0 + 3 * dimY] +
                    v2 * skewMatrix[1 + 3 * dimY] +
                    v3 * skewMatrix[2 + 3 * dimY];

  lookPoint[dimSlice] = OrigDimSliceValue;
}

std::pair<double, double> MANTIDUI_WIDGETS_CORE_PUBLIC
getGridLineAnglesInRadian(Mantid::coord_t skewMatrixCoord[9], size_t dimX,
                          size_t dimY);
}
}

#endif
