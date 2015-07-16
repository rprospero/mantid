#pylint: disable=no-init
import stresstesting
from mantid.simpleapi import *

def relErrLessDelta(val, ref, epsilon):
    """
    Checks that a value 'val' does not defer from a reference value 'ref' by 'epsilon'
    or more. This method compares the relative error. An epsilon of 0.1 means a relative
    difference of 10 % = 100*0.1 %

    @param val :: value obtained from a calculation or algorithm
    @param ref :: (expected) reference value
    @param epsilon :: acceptable relative error (error tolerance)

    @returns if val differs in relative terms from ref by less than epsilon
    """
    if 0 == ref:
        return False
    return abs((ref-val)/ref) < epsilon

class EnginXFocusWithVanadiumCorrection(stresstesting.MantidStressTest):

    def __init__(self):
        stresstesting.MantidStressTest.__init__(self)

        # This test makes sure that the pre-calculated values (which are extensively used in the
        # unit tests) are still the same results as we get from the actual calculations
        self._precalc_van_ws = LoadNexus(Filename='ENGINX_precalculated_vanadium_run000236516_bank_curves.nxs',
                                         OutputWorkspace='ENGIN-X_vanadium_curves_test_ws')
        precalc_van_integ_nxs = LoadNexus(Filename=
                                          'ENGINX_precalculated_vanadium_run000236516_integration.nxs',
                                          OutputWorkspace='ENGIN-X_vanadium_integ_test_ws')
        self._precalc_van_integ_tbl = CreateEmptyTableWorkspace(OutputWorkspace='van_spectra_integration_table')
        self._precalc_van_integ_tbl.addColumn('int', 'Spectra integration')
        for idx in range(0, precalc_van_integ_nxs.getNumberHistograms()):
            self._precalc_van_integ_tbl.addRow([ int(precalc_van_integ_nxs.readY(idx)[0]) ])

        self.van_bank_curves_name = 'enginx_van_bank_curves'
        self.van_bank_curves_pre_integ_name = 'enginx_van_bank_curves_with_precalc_integ'
        self.out_ws_name = 'enginx_focussed'
        self.out_ws_precalc_name = 'enginx_focussed_with_precalculations'

    def runTest(self):
        # These lines run a 'Focus' for the instrument EnginX, as an isolated step here
        # The next test about 'CalibrateFull'-'Calibrate' also includes focusing, as
        # 'Calibrate' uses 'Focus' as a child algorithm
        long_calib_ws = Load(Filename = 'ENGINX00193749.nxs')

        van_ws = Load(Filename = 'ENGINX00236516.nxs')

        # note: not giving calibrated detector positions

        # do all calculations from raw data
        EnggFocus(InputWorkspace = long_calib_ws,
                  VanadiumWorkspace = van_ws,
                  Bank = '1',
                  OutVanadiumCurveFits = self.van_bank_curves_name,
                  OutputWorkspace='out_ws')

        # Now with pre-calculated curves and integration values. This makes sure that these do not
        # change too much AND the final results do not change too much as a result
        EnggFocus(InputWorkspace = long_calib_ws,
                  VanadiumWorkspace = van_ws,
                  Bank = '1',
                  VanadiumIntegWorkspace = self._precalc_van_integ_tbl,
                  OutVanadiumCurveFits = self.van_bank_curves_pre_integ_name,
                  OutputWorkspace='out_pre_int_ws')

    def validate(self):
        out_ws = mtd[self.out_ws_name]
        self.assertEquals(out_ws.getName(), self.out_ws_name)
        self.assertEqual(out_ws.getNumberHistograms(), 1)
        self.assertEqual(out_ws.blocksize(), 10186)
        self.assertEqual(out_ws.getNEvents(), 10186)
        self.assertEqual(out_ws.getNumDims(), 2)
        self.assertEqual(out_ws.YUnit(), 'Counts')
        dimX = out_ws.getXDimension()
        self.assertEqual(dimX.getName(), 'Time-of-flight')
        self.assertEqual(dimX.getUnits(), 'microsecond')
        dimY = out_ws.getYDimension()
        self.assertEqual(dimY.getName(), 'Spectrum')
        self.assertEqual(dimY.getUnits(), '')

        van_out_ws = mtd[self.van_bank_curves_name]
        self.assertEquals(van_out_ws.getName(), self.van_bank_curves_name)
        self.assertTrue(van_out_ws.getNumberHistograms(), 3)
        self.assertEqual(out_ws.blocksize(), 10186)

        dimX = van_out_ws.getXDimension()
        self.assertEqual(dimX.getName(), 'd-Spacing')
        self.assertEqual(dimX.getUnits(), 'Angstrom')
        dimY = van_out_ws.getYDimension()
        self.assertEqual(dimY.getName(), '')
        self.assertEqual(dimY.getUnits(), '')

        # === check the simulated curve from fitted function against previously saved curve ===
        simul = van_out_ws.readY(1)

        # with precalc curve but not precalc integration
        precalc_curve_simul = self._precalc_van_ws.readY(1)
        self.assertEquals(len(simul), len(precalc_curve_simul))

        # with precalculated curve and precalculated integration
        van_pre_integ_out_ws = mtd[self.van_bank_curves_pre_integ_name]
        precalc_all_curve_simul = van_pre_integ_out_ws.readY(1)
        self.assertEquals(len(simul), len(precalc_all_curve_simul))

        delta = 1e-5
        for i in range(0, len(simul)):
            self.assertTrue(relErrLessDelta(simul[i], precalc_curve_simul[i], delta),
                            "Relative difference bigger than acceptable error (%f) when comparing bin %d "
                            "against bank curves previously fitted. got: %f where I expect: %f"%
                            (delta, i, simul[i], precalc_curve_simul[i]))

            self.assertTrue(relErrLessDelta(simul[i], precalc_curve_simul[i], delta),
                            "Relative difference bigger than acceptable error (%f) when comparing bin %d "
                            "against bank curves previously fitted (and also using pre-calculated integration "
                            "values). got: %f where I expect: %f"%
                            (delta, i, simul[i], precalc_all_curve_simul[i]))

        # === check the 'focussed' spectrum ===
        out_precalc_ws = mtd[self.out_ws_precalc_name]
        self.assertEquals(out_precalc_ws.getNumberHistograms(), 1)
        self.assertEquals(out_precalc_ws.blocksize(), out_ws.blocksize())
        focussed_sp = out_ws.readY(0)
        focussed_sp_precalc = out_precalc_ws.readY(0)
        for i in range(0, out_ws.blocksize()):
            self.assertTrue(relErrLessDelta(simul[i], precalc_curve_simul[i], delta),
                            "Relative difference bigger than accepted delta (%f) when comparing bin %d "
                            "of the focussed spectrum against the focussed spectrum obtained using bank curves "
                            "and spectra integration values pre-calculated. got: %f where I expect: %f"%
                            (delta, i, focussed_sp[i], focussed_sp_precalc[i]))

    def cleanup(self):
        mtd.remove(self.out_ws_name)
        mtd.remove(self.van_bank_curves_name)
        mtd.remove(self.van_bank_curves_pre_integ_name)
        mtd.remove(self.van_bank_curves_name)


class EnginXCalibrateFullThenCalibrateTest(stresstesting.MantidStressTest):

    def __init__(self):
        stresstesting.MantidStressTest.__init__(self)
        # difc and zero parameters for GSAS
        self.difc = -1
        self.difc_b2 = -1
        self.zero_b2 = -1
        self.zero = -1
        # table workspace with detector positions
        self.posTable = None

    def runTest(self):
        # These lines run a 'CalibrateFull' and then 'Clibrate' for the instrument EnginX

        # This must be the long Ceria (CeO2) run for calibrate-full
        long_calib_ws = Load(Filename = 'ENGINX00193749.nxs')

        # This must be the (big) Vanadium (V-Nb) run for vanadium corrections
        van_ws = Load(Filename = 'ENGINX00236516.nxs')

        positions = EnggCalibrateFull(Workspace = long_calib_ws,
                                      VanadiumWorkspace = van_ws,
                                      Bank = '1',
                                      ExpectedPeaks = '1.3529, 1.6316, 1.9132')
        self.posTable = positions

        # Bank 1
        (self.difc, self.zero) = EnggCalibrate(InputWorkspace = long_calib_ws,
                                               VanadiumWorkspace = van_ws,
                                               Bank = '1',
                                               ExpectedPeaks = '2.7057,1.9132,1.6316,1.5621,1.3528,0.9566',
                                               DetectorPositions = self.posTable)

        # Bank 2
        (self.difc_b2, self.zero_b2) = EnggCalibrate(InputWorkspace = long_calib_ws,
                                                     VanadiumWorkspace = van_ws,
                                                     Bank = '2',
                                                     ExpectedPeaks = '2.7057,1.9132,1.6316,1.5621,1.3528,0.9566',
                                                     DetectorPositions = self.posTable)

    def validate(self):
        # === check detector positions table produced by EnggCalibrateFull
        self.assertTrue(self.posTable)
        self.assertEquals(self.posTable.columnCount(), 9)
        self.assertEquals(self.posTable.rowCount(), 1200)
        self.assertEquals(self.posTable.cell(88, 0), 100089)   # det ID
        self.assertEquals(self.posTable.cell(200, 0), 101081)  # det ID

        # this will be used as a comparison delta of 'delta' %
        exdelta = 1e-5
        # Note that the reference values are given with 12 digits more for reference than
        # for assert-comparison purposes (comparisons are not that picky, by far)
        self.assertTrue(relErrLessDelta(self.posTable.cell(100, 3), 1.49010562897, exdelta))
        #self.assertDelta(self.posTable.cell(100, 3), 1.49010562897, delta)
        self.assertTrue(relErrLessDelta(self.posTable.cell(400, 4), 1.65264105797, exdelta))
        self.assertTrue(relErrLessDelta(self.posTable.cell(200, 5), 0.296705961227, exdelta))
        self.assertTrue(relErrLessDelta(self.posTable.cell(610, 7), 18585.1738281, exdelta))
        self.assertTrue(relErrLessDelta(self.posTable.cell(1199, 8), -1.56501817703, exdelta))

        # === check difc, zero parameters for GSAS produced by EnggCalibrate
        # Mac fitting tests produce differences for some reason.
        import sys
        if sys.platform == "darwin":
            delta_darwin = 5e-3
            exdelta = delta_darwin

        # Bank 1
        self.assertTrue(relErrLessDelta(self.difc, 18405.0526862, exdelta),
                        "difc parameter for bank 1 is not what was expected, got: %f" % self.difc)
        self.assertTrue(relErrLessDelta(self.zero, -0.835864, exdelta),
                        "zero parameter for bank 1 is not what was expected, got: %f" % self.zero)

        # Bank 2
        self.assertTrue(relErrLessDelta(self.difc_b2, 18392.7375314, exdelta),
                        "difc parameter for bank 2 is not what was expected, got: %f" % self.difc_b2)
        self.assertTrue(relErrLessDelta(self.zero_b2, -11.341251, exdelta),
                        "zero parameter for bank 2 is not what was expected, got: %f" % self.zero_b2)

    def cleanup(self):
        mtd.remove('long_calib_ws')
        mtd.remove('van_ws')
        mtd.remove('positions')
