#ifndef MANTID_MDALGORITHMS_INTEGRATEPEAKS3DFIT_H_
#define MANTID_MDALGORITHMS_INTEGRATEPEAKS3DFIT_H_

#include "MantidAPI/Algorithm.h"
#include "MantidAPI/IMDHistoWorkspace_fwd.h"
#include "MantidDataObjects/PeaksWorkspace.h"
#include "MantidKernel/System.h"
#include "MantidDataObjects/MDHistoWorkspace.h"
#include "MantidAPI/IMDEventWorkspace_fwd.h"
#include "MantidDataObjects/MDEventWorkspace.h"

namespace Mantid {
namespace MDAlgorithms {

/** Integrate single-crystal peaks in reciprocal-space.
 *
 * @author Vickie Lynch
 * @date 2016-06-23
 */
class DLLExport IntegratePeaks3DFit : public API::Algorithm {
public:
  /// Algorithm's name for identification
  const std::string name() const override { return "IntegratePeaks3DFit"; };
  /// Summary of algorithms purpose
  const std::string summary() const override {
    return "Integrate single-crystal peaks in reciprocal space, for "
           "MDHistoWorkspaces.";
  }

  /// Algorithm's version for identification
  int version() const override { return 1; };
  /// Algorithm's category for identification
  const std::string category() const override { return "MDAlgorithms\\Peaks"; }

private:
  /// Initialise the properties
  void init() override;
  /// Run the algorithm
  void exec() override;

  DataObjects::MDHistoWorkspace_sptr
  normalize(double Qx, double Qy, double Qz, double box, int gridPts,
            const API::MatrixWorkspace_sptr &flux,
            const API::MatrixWorkspace_sptr &sa,
            const API::IMDEventWorkspace_sptr &ws);
  DataObjects::MDHistoWorkspace_sptr binEvent(double Qx, double Qy, double Qz, double box,
                                              int gridPts,
                                              const API::IMDWorkspace_sptr &ws);
  DataObjects::MDHistoWorkspace_sptr
  cropHisto(double Qx, double Qy, double Qz, double box, const API::IMDWorkspace_sptr &ws);
  void integratePeak(const int neighborPts,
                     DataObjects::MDHistoWorkspace_sptr out, double &intensity,
                     double &errorSquared);
};

} // namespace Mantid
} // namespace DataObjects

#endif /* MANTID_MDALGORITHMS_INTEGRATEPEAKS3DFIT_H_ */
