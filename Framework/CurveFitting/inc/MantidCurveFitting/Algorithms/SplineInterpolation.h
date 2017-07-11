#ifndef MANTID_CURVEFITTING_SPLINEINTERPOLATION_H_
#define MANTID_CURVEFITTING_SPLINEINTERPOLATION_H_

#include "MantidAPI/Algorithm.h"
#include "MantidCurveFitting/Functions/CubicSpline.h"

namespace Mantid {
namespace CurveFitting {
namespace Algorithms {

using namespace Mantid::API;
using namespace Mantid::CurveFitting::Functions;

/** Takes two workspaces as input. One contain a set of points which define a
  spline,
  and one which contains a number of spectra to be interpolated against spline.

  Produces an output workspace containing the interpolated points

  Optionally, the algorithm can perform a linear interpolation, if the
  WorkspaceToInterpolate contains two points exactly.

  Optionally the algorithm will also produce a grouped workspace of derivatives
  of up to order 2
  for each of the interpolated points.

  @author Samuel Jackson, STFC
  @date 25/07/2013

  Copyright &copy; 2013 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
  National Laboratory & European Spallation Source

  This file is part of Mantid.

  Mantid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  Mantid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  File change history is stored at: <https://github.com/mantidproject/mantid>
  Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class DLLExport SplineInterpolation : public API::Algorithm {
public:
  SplineInterpolation() = default;

  /// Algorithm's name for identification. @see Algorithm::name
  const std::string name() const override;
  /// Algorithm's version for identification. @see Algorithm::version
  int version() const override;
  /// Algorithm's category for identification. @see Algorithm::category
  const std::string category() const override;
  /// Summary of algorithm's purpose. @see Algorithm::summary
  const std::string summary() const override {
    return "Interpolates a set of spectra onto a cubic spline defined by a "
           "second input workspace. Optionally, calculates "
           "derivatives of order 1 or 2 as a side product. Can perform "
           "linear interpolation if the WorkspaceToInterpolate has only two "
           "points. X-axes should be sorted ascending. "
           "If the input workspaces are histograms they will be converted to "
           "point data. Outside the interpolation range it will perform flat "
           "extrapolation. ";
  }
  /// Cross-check properties with each other @see IAlgorithm::validateInputs
  std::map<std::string, std::string> validateInputs() override;

private:
  void init() override;
  void exec() override;

  std::unique_ptr<BackgroundFunction> m_cspline;

  /// setup an output workspace using meta data from inws and taking a number of
  /// spectra
  API::MatrixWorkspace_sptr
      setupOutputWorkspace(API::MatrixWorkspace_sptr,
                           API::MatrixWorkspace_sptr) const;

  /// convert a binned workspace to point data using ConvertToPointData
  MatrixWorkspace_sptr convertBinnedData(MatrixWorkspace_sptr);

  /// set the points that define the spline used for interpolation of a
  /// workspace
  void setInterpolationPoints(MatrixWorkspace_const_sptr, const int) const;

  /// Calculate the interpolation of the input workspace against the spline and
  /// store it in outputWorkspace
  void calculateSpline(MatrixWorkspace_const_sptr, MatrixWorkspace_sptr,
                       const int) const;

  /// Calculate the derivatives of the input workspace from the spline.
  void calculateDerivatives(MatrixWorkspace_const_sptr, MatrixWorkspace_sptr,
                            const int) const;

  /// Check if an x value falls within the range of the spline
  void extrapolateFlat(MatrixWorkspace_sptr, MatrixWorkspace_const_sptr) const;
};

} // namespace Algorithms
} // namespace CurveFitting
} // namespace Mantid

#endif /* MANTID_CURVEFITTING_SPLINEINTERPOLATION_H_ */
