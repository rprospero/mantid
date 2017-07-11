#include "MantidCurveFitting/Algorithms/SplineInterpolation.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/NumericAxis.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/WorkspaceGroup.h"
#include "MantidKernel/BoundedValidator.h"
#include "MantidKernel/make_unique.h"

#include <sstream>

namespace Mantid {
namespace CurveFitting {
namespace Algorithms {

// Register the algorithm into the AlgorithmFactory
DECLARE_ALGORITHM(SplineInterpolation)

using namespace Kernel;

//----------------------------------------------------------------------------------------------
/// Algorithm's name for identification. @see Algorithm::name
const std::string SplineInterpolation::name() const {
  return "SplineInterpolation";
}

/// Algorithm's version for identification. @see Algorithm::version
int SplineInterpolation::version() const { return 1; }

/// Algorithm's category for identification. @see Algorithm::category
const std::string SplineInterpolation::category() const {
  return "Optimization;CorrectionFunctions\\BackgroundCorrections";
}

//----------------------------------------------------------------------------------------------
/** Initialize the algorithm's properties.
 */
void SplineInterpolation::init() {
  declareProperty(make_unique<WorkspaceProperty<>>("WorkspaceToMatch", "",
                                                   Direction::Input),
                  "The workspace which defines the points of the spline.");

  declareProperty(
      make_unique<WorkspaceProperty<>>("WorkspaceToInterpolate", "",
                                       Direction::Input),
      "The workspace on which to perform the interpolation algorithm.");

  declareProperty(
      make_unique<WorkspaceProperty<>>("OutputWorkspace", "",
                                       Direction::Output),
      "The workspace containing the calculated points and derivatives");

  declareProperty(make_unique<WorkspaceProperty<WorkspaceGroup>>(
                      "OutputWorkspaceDeriv", "", Direction::Output,
                      PropertyMode::Optional),
                  "The workspace containing the calculated derivatives");

  auto validator = boost::make_shared<BoundedValidator<int>>(0, 2);
  declareProperty("DerivOrder", 0, validator,
                  "Order to derivatives to calculate (default 0 "
                  "omits calculation)");

  declareProperty("Linear2Points", false,
                  "Set to true to perform linear interpolation for 2 points "
                  "instead.");
}

//----------------------------------------------------------------------------------------------
/** Input validation for the WorkspaceToInterpolate
 * If more than two points given, perform cubic spline interpolation
 * If only two points given, check if Linear2Points is true and if true,
 * continue
 * If one point is given interpolation does not make sense
  */
std::map<std::string, std::string> SplineInterpolation::validateInputs() {
  // initialise map (result)
  std::map<std::string, std::string> result;

  // get inputs that need validation
  const bool lin2pts = getProperty("Linear2Points");

  MatrixWorkspace_const_sptr iwsValid = getProperty("WorkspaceToInterpolate");
  const size_t binsNo = iwsValid->blocksize();

  // The minimum number of points for cubic splines is 3,
  // used and set by function CubicSpline as well
  switch (binsNo) {
  case 1:
    result["WorkspaceToInterpolate"] =
        "Workspace must have minimum two points.";
  case 2:
    if (lin2pts == false) {
      result["WorkspaceToInterpolate"] =
          "Workspace has only 2 points, "
          "you can enable linear interpolation by "
          "setting the property Linear2Points. Otherwise "
          "provide a minimum of 3 points.";
    }
  }

  const int derivOrder = getProperty("DerivOrder");
  const std::string derivName = getProperty("OutputWorkspaceDeriv");
  if (derivName.empty() && (derivOrder > 0)) {
    result["OutputWorkspaceDeriv"] =
        "Enter a name for the OutputWorkspaceDeriv "
        "or set DerivOrder to zero.";
  }

  return result;
}

//----------------------------------------------------------------------------------------------
/** Execute the algorithm.
 */
void SplineInterpolation::exec() {
  // read in algorithm parameters
  const int order = static_cast<int>(getProperty("DerivOrder"));
  const bool linear = getProperty("Linear2Points");

  // set input workspaces
  MatrixWorkspace_sptr mws = getProperty("WorkspaceToMatch");
  MatrixWorkspace_sptr iws = getProperty("WorkspaceToInterpolate");

  // first convert binned data to point data
  mws = convertBinnedData(mws);
  iws = convertBinnedData(iws);

  const size_t histNo = iws->getNumberHistograms();
  const size_t binsNo = iws->blocksize();
  const size_t size = mws->blocksize();

  // vector of multiple derivative workspaces
  std::vector<MatrixWorkspace_sptr> derivs(histNo);

  // warn user that we only use first spectra in matching workspace
  if (mws->getNumberHistograms() > 1) {
    g_log.warning()
        << "Algorithm can only interpolate against a single data set. "
           "Only the x-axis of the first spectrum will be used.\n";
  }

  MatrixWorkspace_sptr outputWorkspace = setupOutputWorkspace(mws, iws);

  Progress pgress(this, 0.0, 1.0, histNo);

  if (linear && binsNo == 2) {
    g_log.information() << "Performing linear interpolation.\n";

    for (int i = 0; i < static_cast<int>(histNo); ++i) {
      // set up the function that needs to be interpolated
      std::unique_ptr<gsl_interp_accel, void (*)(gsl_interp_accel *)> acc(
          gsl_interp_accel_alloc(), gsl_interp_accel_free);
      std::unique_ptr<gsl_interp, void (*)(gsl_interp *)> linear(
          gsl_interp_alloc(gsl_interp_linear, binsNo), gsl_interp_free);
      gsl_interp_linear->init(linear.get(), &(iws->x(i)[0]), &(iws->y(i)[0]),
                              binsNo);

      for (int k = 0; k < static_cast<int>(size); ++k) {
        gsl_interp_linear->eval(linear.get(), &(iws->x(i)[0]), &(iws->y(i)[0]),
                                binsNo, mws->x(0)[k], acc.get(),
                                &(outputWorkspace->mutableY(i)[k]));
        if (order > 0) {
          derivs[i] =
              WorkspaceFactory::Instance().create(iws, order, size, size);
          auto vAxis = new NumericAxis(order);
          for (int j = 0; j < order; ++j) {
            derivs[i]->setSharedX(j, mws->sharedX(0));
            vAxis->setValue(j, j + 1);
            if (j == 0)
              gsl_interp_linear->eval_deriv(
                  linear.get(), &(iws->x(i)[0]), &(iws->y(i)[0]), binsNo,
                  mws->x(0)[k], acc.get(), &(derivs[i]->mutableY(i)[k]));
            if (j == 1)
              gsl_interp_linear->eval_deriv2(
                  linear.get(), &(iws->x(i)[0]), &(iws->y(i)[0]), binsNo,
                  mws->x(0)[k], acc.get(), &(derivs[i]->mutableY(i)[k]));
          }
          derivs[i]->replaceAxis(1, vAxis);
        }
      }
    }
  } else {
    g_log.information() << "Performing cubic spline interpolation.\n";

    for (int i = 0; i < static_cast<int>(histNo); ++i) {
      // Create and instance of the cubic spline function
      m_cspline = make_unique<CubicSpline>();
      // set the interpolation points
      setInterpolationPoints(iws, i);
      // compare the data set against our spline
      calculateSpline(mws, outputWorkspace, i);
      outputWorkspace->setSharedX(i, mws->sharedX(0));

      // check if we want derivatives
      if (order > 0) {
        auto vAxis2 = new NumericAxis(order);
        derivs[i] = WorkspaceFactory::Instance().create(iws, order, size, size);

        // calculate the derivatives for each order chosen
        for (int j = 0; j < order; ++j) {
          vAxis2->setValue(j, j + 1);
          calculateDerivatives(mws, derivs[i], j + 1);
          derivs[i]->setSharedX(j, mws->sharedX(0));
        }
        derivs[i]->replaceAxis(1, vAxis2);
      }
      pgress.report();
    }
  }

  // store the output workspaces
  if (order > 0) {
    // Store derivatives in a grouped workspace
    WorkspaceGroup_sptr wsg = boost::make_shared<WorkspaceGroup>();
    for (int i = 0; i < static_cast<int>(histNo); ++i) {
      wsg->addWorkspace(derivs[i]);
    }
    // set y values according to interpolation range must be set to zero
    setProperty("OutputWorkspaceDeriv", wsg);
  }

  // set y values according to the interpolation range
  extrapolateFlat(outputWorkspace, iws);
  setProperty("OutputWorkspace", outputWorkspace);
}

/** Copy the meta data for the input workspace to an output workspace and create
 *it with the desired number of spectra.
 * Also labels the axis of each spectra with Yi, where i is the index
 *
 * @param mws :: The input workspace to match
 * @param iws :: The input workspace to interpolate
 * @return The pointer to the newly created workspace
 */
API::MatrixWorkspace_sptr
SplineInterpolation::setupOutputWorkspace(API::MatrixWorkspace_sptr mws,
                                          API::MatrixWorkspace_sptr iws) const {
  const size_t numSpec = iws->getNumberHistograms();
  MatrixWorkspace_sptr outputWorkspace =
      WorkspaceFactory::Instance().create(mws, numSpec);

  // Use the vertical axis form the workspace to interpolate on the output WS
  Axis *vAxis = iws->getAxis(1)->clone(mws.get());
  outputWorkspace->replaceAxis(1, vAxis);

  return outputWorkspace;
}

/** Convert a binned workspace to point data
 *
 * @param workspace :: The input workspace
 * @return The converted workspace containing point data
 */
MatrixWorkspace_sptr
SplineInterpolation::convertBinnedData(MatrixWorkspace_sptr workspace) {
  if (workspace->isHistogramData()) {
    g_log.warning("Histogram data provided, converting to point data");
    Algorithm_sptr converter = createChildAlgorithm("ConvertToPointData");
    converter->initialize();
    converter->setProperty("InputWorkspace", workspace);
    converter->execute();
    return converter->getProperty("OutputWorkspace");
  } else {
    return workspace;
  }
}

/** Sets the points defining the spline
 *
 * @param inputWorkspace :: The input workspace containing the points of the
 *spline
 * @param row :: The row of spectra to use
 */
void SplineInterpolation::setInterpolationPoints(
    MatrixWorkspace_const_sptr inputWorkspace, const int row) const {
  const auto &xIn = inputWorkspace->x(row);
  const auto &yIn = inputWorkspace->y(row);
  int size = static_cast<int>(xIn.size());

  // pass x attributes and y parameters to CubicSpline
  m_cspline->setAttributeValue("n", size);

  for (int i = 0; i < size; ++i) {
    std::string xName = "x" + std::to_string(i);
    m_cspline->setAttributeValue(xName, xIn[i]);
    // Call parent setParameter implementation
    m_cspline->ParamFunction::setParameter(i, yIn[i], true);
  }
}

/** Calculate the derivatives of the given order from the interpolated points
 *
 * @param inputWorkspace :: The input workspace
 * @param outputWorkspace :: The output workspace
 * @param order :: The order of derivatives to calculate
 */
void SplineInterpolation::calculateDerivatives(
    MatrixWorkspace_const_sptr inputWorkspace,
    MatrixWorkspace_sptr outputWorkspace, const int order) const {
  // get x and y parameters from workspaces
  const size_t nData = inputWorkspace->y(0).size();
  const double *xValues = &(inputWorkspace->x(0)[0]);
  double *yValues = &(outputWorkspace->mutableY(order - 1)[0]);

  // calculate the derivatives
  m_cspline->derivative1D(yValues, xValues, nData, order);
}

/** Calculate the interpolation of the input points against the spline
 *
 * @param inputWorkspace :: The input workspace
 * @param outputWorkspace :: The output workspace
 * @param row :: The row of spectra to use
 */
void SplineInterpolation::calculateSpline(
    MatrixWorkspace_const_sptr inputWorkspace,
    MatrixWorkspace_sptr outputWorkspace, int row) const {
  // setup input parameters
  const size_t nData = inputWorkspace->y(0).size();
  const double *xValues = &(inputWorkspace->x(0)[0]);
  double *yValues = &(outputWorkspace->mutableY(row)[0]);

  // calculate the interpolation
  m_cspline->function1D(yValues, xValues, nData);
}

/** Flat extrapolates the points that are outside of the x-axis of workspace to
 * interpolate.
 *
 * @param outputWorkspace :: The output workspace
 * @param interpolationWorkspace :: The workspace to interpolate
 */
void SplineInterpolation::extrapolateFlat(
    MatrixWorkspace_sptr outputWorkspace,
    MatrixWorkspace_const_sptr interpolationWorkspace) const {
  // setup input parameters
  const size_t histNo = outputWorkspace->getNumberHistograms();
  const size_t nData = outputWorkspace->blocksize();
  // this is the x-axis of the first spectrum workspace to match
  const auto &xValues = outputWorkspace->x(0).rawData();

  for (size_t n = 0; n < histNo; ++n) {
    const auto &xInterValues = interpolationWorkspace->x(n).rawData();
    const auto &yRef = interpolationWorkspace->y(n).rawData();
    const auto &yValues = outputWorkspace->mutableY(n).rawData();

    const auto first =
        std::upper_bound(xValues.cbegin(), xValues.cend(), xInterValues[0]);
    const long leftExtra = std::distance(first, xValues.cbegin());

    const auto last = std::upper_bound(xValues.crbegin(), xValues.crend(),
                                       xInterValues[nData - 1]);
    const long rightExtra = std::distance(last, xValues.crbegin());

    std::stringstream log;
    log << "Workspace index " << n << ": flat extrapolating first " << leftExtra
        << " and last " << rightExtra << " bins.";
    g_log.warning(log.str());

    //std::fill_n(yValues.begin(), leftExtra, yRef.cbegin());

    //std::fill_n(yValues.rbegin(), rightExtra, yRef.crbegin());
  }
}
} // namespace Algorithms
} // namespace CurveFitting
} // namespace Mantid
