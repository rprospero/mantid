#ifndef MANTID_CURVEFITTING_PRODUCTFUNCTION_H_
#define MANTID_CURVEFITTING_PRODUCTFUNCTION_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/IFunction.h"
#include "MantidAPI/AssociativeCompositeFunction.h"
#include <boost/shared_array.hpp>
#include <cmath>

namespace Mantid {
namespace CurveFitting {
namespace Functions {
/**
Allow user to create a fit function which is the product of two or
more other fit functions.

@author Anders Markvardsen, ISIS, RAL
@date 4/4/2011

Copyright &copy; 2007-8 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
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
class DLLExport ProductFunction : public API::AssociativeCompositeFunction {
public:
  /// overwrite IFunction base class methods
  std::string name() const override { return "ProductFunction"; }
  /// if f is of the same class then its component functions
  /// are treated separately
  bool isAssociative(API::IFunction_sptr f) const override;
  /// Function you want to fit to.
  /// @param domain :: The space on which the function acts
  /// @param values :: The buffer for writing the calculated values. Must be big
  /// enough to accept dataSize() values
  void function(const API::FunctionDomain &domain,
                API::FunctionValues &values) const override;
  /// Calculate the derivatives
  void functionDeriv(const API::FunctionDomain &domain,
                     API::Jacobian &jacobian) override;

protected:
  /// overwrite IFunction base class method, which declare function parameters
  void init() override{};
};

} // namespace Functions
} // namespace CurveFitting
} // namespace Mantid

#endif /*MANTID_CURVEFITTING_PRODUCTFUNCTIONMW_H_*/
