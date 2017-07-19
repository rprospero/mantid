"""
    Test of FunctiomWrapper and related classes
"""
from __future__ import (absolute_import, division, print_function)

import unittest
import testhelpers
import platform
from mantid.simpleapi import CreateWorkspace, EvaluateFunction, Fit, FitDialog 
from mantid.simpleapi import FunctionWrapper, CompositeFunctionWrapper, ProductFunctionWrapper, ConvolutionWrapper, MultiDomainFunctionWrapper 
from mantid.simpleapi import Gaussian, LinearBackground, Polynomial
from mantid.api import mtd, MatrixWorkspace, ITableWorkspace
import numpy as np
from testhelpers import run_algorithm

class FunctionWrapperTest(unittest.TestCase):

    _raw_ws = None

    def setUp(self):
        pass
        
    def test_creation(self):
        testhelpers.assertRaisesNothing(self, FunctionWrapper, "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        
    def test_read_array_elements(self):
        g = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        self.assertAlmostEqual(g["Height"],7.5,10)
        self.assertAlmostEqual(g[2],1.2,10)
        
    def test_write_array_elements(self):
        g = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10) 
        g["Height"] = 8
        self.assertAlmostEqual(g["Height"],8,10)
        g[2] = 1.5
        self.assertAlmostEqual(g[2],1.5,10)
        
    def test_compositefunction_creation(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        testhelpers.assertRaisesNothing(self, CompositeFunctionWrapper, g0, g1)  
        
    def test_copy_on_compositefunction_creation(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper( g0, g1 )
        g0["Height"] = 10.0
        g1["Height"] = 11.0
        # Check that the composite function remains unmodified.
        self.assertAlmostEqual( c["f0.Height"],7.5)
        self.assertAlmostEqual( c["f1.Height"],8.5)

    def test_compositefunction_read_array_elements(self):
        lb = FunctionWrapper("LinearBackground", A0=0.5, A1=1.5)
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper( lb, g0, g1 )
        
        self.assertAlmostEqual(c["f0.A1"], 1.5,10)      
        self.assertAlmostEqual(c["f1.Height"], 7.5,10)
        self.assertAlmostEqual(c["f2.Height"], 8.5,10)

        self.assertAlmostEqual(c[0]["A1"], 1.5,10)        
        self.assertAlmostEqual(c[1]["Height"], 7.5,10)
        self.assertAlmostEqual(c[2]["Height"], 8.5,10)
        
    def test_compositefunction_write_array_elements(self):
        lb = FunctionWrapper("LinearBackground", A0=0.5, A1=1.5)
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper( lb, g0, g1 )
             
        c["f0.A1"] = 0.0
        self.assertAlmostEqual(c["f0.A1"], 0.0,10) 
        c[0]["A1"] = 1.0
        self.assertAlmostEqual(c["f0.A1"], 1.0,10)
        
        c["f1.Height"] = 10.0
        self.assertAlmostEqual(c[1]["Height"], 10.0,10) 
        c[1]["Height"] = 11.0
        self.assertAlmostEqual(c[1]["Height"], 11.0,10)
        
    def test_attributes(self):
        testhelpers.assertRaisesNothing(self, FunctionWrapper, "Polynomial", attributes={'n': 3}, A0=4, A1=3, A2=2, A3=1)
        testhelpers.assertRaisesNothing(self, FunctionWrapper, "Polynomial", n=3, A0=4, A1=3, A2=2, A3=1)
        
    def test_fix(self):
        g = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=15)
        
        g.fix("Sigma")
        g_str = str(g)
        self.assertEqual(g_str.count("ties="),1)
        self.assertEqual(g_str.count("ties=(Sigma=1.2)"),1)
        
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper(g0, g1)
        
        c.fix("f1.Sigma")
        c_str = str(c)
        self.assertEqual(c_str.count("ties="),1)
        self.assertEqual(c_str.count("ties=(Sigma=1.2"),1)
        
        # remove non-existent tie and test it has no effect
        c.untie("f1.Height")
        cu_str = str(c)
        self.assertEqual(c_str, cu_str)
        
        # remove actual tie
        c.untie("f1.Sigma")
        cz_str = str(c)
        self.assertEqual(cz_str.count("ties="),0)
        
    def test_fix_all(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper(g0, g1)
        
        c.fixAll("Sigma")
        c_str = str(c)
        self.assertEqual(c_str.count("ties="),2)
        self.assertEqual(c_str.count("ties=(Sigma="),2)
        
        # remove non-existent ties and test it has no effect
        c.untieAll("Height")
        cu_str = str(c)
        self.assertEqual(c_str, cu_str)
        
        # remove actual ties
        c.untieAll("Sigma")
        cz_str = str(c)
        self.assertEqual(cz_str.count("ties="),0)
        
    def test_fix_all_parameters(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        c = CompositeFunctionWrapper(g0, g1)
        
        c.fixAllParameters()
        c_str = str(c)
        self.assertEqual(c_str.count("ties="),2)
        self.assertEqual(c_str.count("ties=(Height=7.5,PeakCentre=10,Sigma=1.2)"),2)
        
        c.untieAllParameters()
        cz_str = str(c)
        self.assertEqual(cz_str.count("ties="),0)
    

    def test_tie(self):
        g = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=15)
        
        g.tie(Sigma="0.1*Height")
        g_str = str(g)
        self.assertEqual(g_str.count("ties="),1)
        self.assertEqual(g_str.count("ties=(Sigma=0.1*Height)"),1)
        
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper(g0, g1)
        
        c.tie({"f1.Sigma":"f0.Sigma"})
        c_str = str(c)
        self.assertEqual(c_str.count("ties="),1)
        self.assertEqual(c_str.count("ties=(f1.Sigma=f0.Sigma)"),1)
        
        c.untie("f1.Sigma")
        cz_str = str(c)
        self.assertEqual(cz_str.count("ties="),0)
        
    def test_tie_all(self):
    
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper(g0, g1)
        
        c.tieAll("Sigma")
        c_str = str(c)
        self.assertEqual(c_str.count("ties="),1)
        self.assertEqual(c_str.count("ties=(f1.Sigma=f0.Sigma)"),1)
        
        c.untieAll("Sigma")
        cz_str = str(c)
        self.assertEqual(cz_str.count("ties="),0)
        
    def test_constrain(self):
        g = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=15)
        
        g.constrain("Sigma < 2.0, Height > 7.0")
        g_str = str(g)
        self.assertEqual(g_str.count("constraints="),1)
        self.assertEqual(g_str.count("Sigma<2"),1)
        self.assertEqual(g_str.count("7<Height"),1)

        
        g.unconstrain("Height")
        g1_str = str(g)
        self.assertEqual(g1_str.count("constraints="),1)
        self.assertEqual(g1_str.count("constraints=(Sigma<2)"),1)
        
        g.unconstrain("Sigma")
        gz_str = str(g)
        self.assertEqual(gz_str.count("constraints="),0)
        
    def test_constrain_composite(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        c = CompositeFunctionWrapper(g0, g1)
        
        c.constrain("f1.Sigma < 2, f0.Height > 7")
        c_str = str(c)
        self.assertEqual(c_str.count("f1.Sigma<2"),1)
        self.assertEqual(c_str.count("7<f0.Height"),1)
        
        c.unconstrain("f1.Sigma")
        c.unconstrain("f0.Height")
        cz_str = str(c)
        self.assertEqual(cz_str.count("Constraints="),0)
        
    def test_constrainall(self):
        lb = FunctionWrapper("LinearBackground", A0=0.5, A1=1.5)
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        g2 = FunctionWrapper( "Gaussian", Height=9.5, Sigma=1.2, PeakCentre=12)
        c0 = CompositeFunctionWrapper( g1, g2 )
        c = CompositeFunctionWrapper( lb, g0, c0)
        c.constrainAll("Sigma < 1.8")
        
        c_str = str(c)
        self.assertEqual(c_str.count("constraints="),3)
        self.assertEqual(c_str.count("Sigma<1.8"),3)
        
        lb_str = str(c[0])
        self.assertEqual(lb_str.count("constraints="),0)
        
        g0_str = str(c[1])
        self.assertEqual(g0_str.count("constraints="),1)
        self.assertEqual(g0_str.count("Sigma<1.8"),1)
        
        g1_str = str(c[2][0])
        self.assertEqual(g1_str.count("constraints="),1)
        self.assertEqual(g1_str.count("Sigma<1.8"),1)
        
        g2_str = str(c[2][1])
        self.assertEqual(g2_str.count("constraints="),1)
        self.assertEqual(g2_str.count("Sigma<1.8"),1)
        
        c.unconstrainAll("Sigma")
        
        cz_str = str(c)
        self.assertEqual(cz_str.count("constraints="),0)
              
    def test_free(self):
        g = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=15)
        
        g.constrain("Sigma < 2.0, Height > 7.0")
        g.tie({"PeakCentre":"2*Height"})
        
        g.free("Height")
        g1_str = str(g)
        self.assertEqual(g1_str.count("ties="),1)
        self.assertEqual(g1_str.count("constraints="),1)
        self.assertEqual(g1_str.count("constraints=(Sigma<2)"),1)
        
        g.free("PeakCentre")
        g2_str = str(g)
        self.assertEqual(g2_str.count("ties="),0)
        self.assertEqual(g2_str.count("constraints="),1)
        
        g.free("Sigma")
        gz_str = str(g)
        self.assertEqual(gz_str.count("constraints="),0)
        
    def test_flatten(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.25, PeakCentre=12)  
        g2 = FunctionWrapper( "Gaussian", Height=9.5, Sigma=1.3, PeakCentre=14)
        l = FunctionWrapper("Lorentzian",PeakCentre=9, Amplitude=2.4, FWHM=3)
        lb = FunctionWrapper("LinearBackground")
        
        # Test already flat composite function, no change should occur
        c1 = CompositeFunctionWrapper(lb, g0, g1 )
        fc1 = c1.flatten()
        c1_str = str(c1)
        fc1_str = str(fc1)
        self.assertEqual(fc1_str,c1_str)
        
        # Test composite function of depth 1
        c2 = CompositeFunctionWrapper(c1, l)
        fc2 = c2.flatten()
        fc2_str = str(fc2)
        self.assertEqual(fc2_str.count("("),0)
        self.assertEqual(fc2_str.count("PeakCentre"),3)
        self.assertEqual(fc2_str.count("Sigma="),2)
        self.assertEqual(fc2_str.count("Sigma=1.25"),1)
        
        # Test composite function of depth 2
        c3 = CompositeFunctionWrapper( g2, c2)
        fc3 = c3.flatten()
        fc3_str = str(fc3)
        self.assertEqual(fc3_str.count("("),0)
        self.assertEqual(fc3_str.count("PeakCentre"),4)
        self.assertEqual(fc3_str.count("Sigma="),3)
        self.assertEqual(fc3_str.count("Sigma=1.25"),1)
        self.assertEqual(fc3_str.count("Sigma=1.3"),1)
        
    def test_add(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.25, PeakCentre=12)  
        lb = FunctionWrapper("LinearBackground")
        
        c = lb + g0 + g1
        
        self.assertTrue( isinstance( c, CompositeFunctionWrapper) )
        c_str = str(c)
        # self.assertEqual(c_str.count("("),0)
        self.assertEqual(c_str.count("LinearBackground"),1)
        self.assertEqual(c_str.count("Gaussian"),2)
        
        #lb_str = str(lb)
        #c0_str = str(c[0])
        #self.assertEqual(c0_str, lb_str)
           
        #g0_str = str(g0)
        #c1_str = str(c[1])
        #self.assertEqual(c1_str, g0_str)
        
        #g1_str = str(g1)
        #c2_str = str(c[2])
        #self.assertEqual(c2_str, g1_str)
        
    def test_incremental_add(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.25, PeakCentre=12)  
        lb = FunctionWrapper("LinearBackground")
        
        c = CompositeFunctionWrapper( lb, g0)
        c += g1
        c_str = str(c)
        self.assertEqual(c_str.count("("),0)
        self.assertEqual(c_str.count("LinearBackground"),1)
        self.assertEqual(c_str.count("Gaussian"),2)
        
        lb_str = str(lb)
        c0_str = str(c[0])
        self.assertEqual(c0_str, lb_str)
           
        g0_str = str(g0)
        c1_str = str(c[1])
        self.assertEqual(c1_str, g0_str)
        
        g1_str = str(g1)
        c2_str = str(c[2])
        self.assertEqual(c2_str, g1_str)

    def test_del(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.25, PeakCentre=12)  
        lb = FunctionWrapper("LinearBackground")
        
        c = CompositeFunctionWrapper( lb, g0, g1)
        del c[1]
        
        c_str = str(c)
        self.assertEqual(c_str.count("("),0)
        self.assertEqual(c_str.count("LinearBackground"),1)
        self.assertEqual(c_str.count("Gaussian"),1)
        self.assertEqual(c_str.count("Height=8.5"),1)
        
    def test_len(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.25, PeakCentre=12)  
        lb = FunctionWrapper("LinearBackground")
        
        c = CompositeFunctionWrapper( lb, g0, g1)
        self.assertEqual( len(c), 3)
        
    def test_iteration(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.25, PeakCentre=12)  
        lb = FunctionWrapper("LinearBackground")
        
        c = CompositeFunctionWrapper( lb, g0, g1)
        count = 0
        for f in c:
           count += 1
        self.assertEqual( count, 3)
        
    def test_productfunction_creation(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        testhelpers.assertRaisesNothing(self, ProductFunctionWrapper, g0, g1)

    def test_mul(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)  
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.25, PeakCentre=12)  
        lb = FunctionWrapper("LinearBackground")
        
        p = lb * g0 * g1
        
        self.assertTrue( isinstance( p, ProductFunctionWrapper) )
        p_str = str(p)
        #self.assertEqual(p_str.count("("),0)
        self.assertEqual(p_str.count("LinearBackground"),1)
        self.assertEqual(p_str.count("Gaussian"),2)
        
        #lb_str = str(lb)
        #p0_str = str(p[0])
        #self.assertEqual(p0_str, lb_str)
           
        #g0_str = str(g0)
        #p1_str = str(p[1])
        #self.assertEqual(p1_str, g0_str)
        
        #g1_str = str(g1)
        #p2_str = str(p[2])
        #self.assertEqual(p2_str, g1_str)   

    def test_convolution_creation(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        testhelpers.assertRaisesNothing(self, ConvolutionWrapper, g0, g1)
        
    def test_multidomainfunction_creation(self):
        g0 = FunctionWrapper( "Gaussian", Height=7.5, Sigma=1.2, PeakCentre=10)
        g1 = FunctionWrapper( "Gaussian", Height=8.5, Sigma=1.2, PeakCentre=11)
        testhelpers.assertRaisesNothing(self, MultiDomainFunctionWrapper, g0, g1)
        m = MultiDomainFunctionWrapper( g0, g1, Global=["Height"])
        self.assertEqual( m.nDomains(), 2)
        m_str = str(m)
        self.assertEqual( m_str.count("ties"),1)
        self.assertEqual( m_str.count("Height"),4) # 2 in functions 2 in ties
        
    def test_generatedfunction(self):
        testhelpers.assertRaisesNothing(self, Gaussian, Height=7.5, Sigma=1.2, PeakCentre=10)
        lb = LinearBackground()
        g = Gaussian(Height=7.5, Sigma=1.2, PeakCentre=10)
        s = lb + g
        self.assertTrue( isinstance( s, CompositeFunctionWrapper) )
    
    def test_evaluation(self):
        l0 = FunctionWrapper( "LinearBackground", A0=0, A1=2)
        l1 = FunctionWrapper( "LinearBackground", A0=5, A1=-1)

        ws = CreateWorkspace(DataX=[0,1,2,3,4], DataY=[5,5,5,5])
        
        c = CompositeFunctionWrapper(l0, l1)
        cws = EvaluateFunction(c,"ws", OutputWorkspace='out')
        cvals = cws.readY(1)
        self.assertAlmostEqual(cvals[0], 5.5)
        self.assertAlmostEqual(cvals[1], 6.5)
        self.assertAlmostEqual(cvals[2], 7.5)
        self.assertAlmostEqual(cvals[3], 8.5)
        
        p = ProductFunctionWrapper(l0, l1)
        pws = EvaluateFunction(p,"ws", OutputWorkspace='out')
        pvals = pws.readY(1)
        self.assertAlmostEqual(pvals[0], 4.5)
        self.assertAlmostEqual(pvals[1], 10.5)
        self.assertAlmostEqual(pvals[2], 12.5)
        self.assertAlmostEqual(pvals[3], 10.5)
        
        sq = Polynomial(attributes={'n': 2}, A0=0, A1=0.0, A2=1.0)
        sqws = EvaluateFunction(sq,"ws", OutputWorkspace='out')
        sqvals = sqws.readY(1)
        self.assertAlmostEqual(sqvals[0], 0.25)
        self.assertAlmostEqual(sqvals[1], 2.25)
        self.assertAlmostEqual(sqvals[2], 6.25)
        
    def test_arithmetic(self):
        l0 = FunctionWrapper( "LinearBackground", A0=0, A1=2)
        l1 = FunctionWrapper( "LinearBackground", A0=5, A1=-1)

        ws = CreateWorkspace(DataX=[0,1], DataY=[5])

        c = CompositeFunctionWrapper(l0, l1)
        p = ProductFunctionWrapper(l0, l1)
        
        s1 = c + p
        s1ws = EvaluateFunction(s1,"ws", OutputWorkspace='out')
        s1vals = s1ws.readY(1)
        self.assertAlmostEqual(s1vals[0], 10.0)
        
        s2 = p + c
        s2ws = EvaluateFunction(s2,"ws", OutputWorkspace='out')
        s2vals = s2ws.readY(1)
        self.assertAlmostEqual(s2vals[0], 10.0)

        
       
if __name__ == '__main__':
    unittest.main()