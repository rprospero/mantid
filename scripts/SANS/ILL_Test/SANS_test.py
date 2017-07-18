sample_ws = Load("001464+001465+001466+002209+002232")
#__dark_current_002227 = Load("002227")
#transmission_sample = Load("002197")
#transmission_empty = Load("002192")
#background = Load("002228")
#background_sample = Load("002193")
#background_empty = Load("002192")
#__beam_finder_002192 = Load("002192")
#sensitivity = Load("002229")

maskedDetectors = [14709,14710,14711,14712,14713,14714,14715,14716,14717,14718,
        14719,14720,14721,14722,14723,14724,14725,14726,14727,14728,14729,14730,14731,
        14732,14733,14734,14735,14965,14966,14967,14968,14969,14970,14971,14972,14973,
        14974,14975,14976,14977,14978,14979,14980,14981,14982,14983,14984,14985,14986,
        14987,14988,14989,14990,14991,15221,15222,15223,15224,15225,15226,15227,15228,
        15229,15230,15231,15232,15233,15234,15235,15236,15237,15238,15239,15240,15241,
        15242,15243,15244,15245,15246,15247,15477,15478,15479,15480,15481,15482,15483,
        15484,15485,15486,15487,15488,15489,15490,15491,15492,15493,15494,15495,15496,
        15497,15498,15499,15500,15501,15502,15503,15733,15734,15735,15736,15737,15738,
        15739,15740,15741,15742,15743,15744,15745,15746,15747,15748,15749,15750,15751,
        15752,15753,15754,15755,15756,15757,15758,15759,15989,15990,15991,15992,15993,
        15994,15995,15996,15997,15998,15999,16000,16001,16002,16003,16004,16005,16006,
        16007,16008,16009,16010,16011,16012,16013,16014,16015,16245,16246,16247,16248,
        16249,16250,16251,16252,16253,16254,16255,16256,16257,16258,16259,16260,16261,
        16262,16263,16264,16265,16266,16267,16268,16269,16270,16271,16501,16502,16503,
        16504,16505,16506,16507,16508,16509,16510,16511,16512,16513,16514,16515,16516,
        16517,16518,16519,16520,16521,16522,16523,16524,16525,16526,16527,16757,16758,
        16759,16760,16761,16762,16763,16764,16765,16766,16767,16768,16769,16770,16771,
        16772,16773,16774,16775,16776,16777,16778,16779,16780,16781,16782,16783,17013,
        17014,17015,17016,17017,17018,17019,17020,17021,17022,17023,17024,17025,17026,
        17027,17028,17029,17030,17031,17032,17033,17034,17035,17036,17037,17038,17039,
        17269,17270,17271,17272,17273,17274,17275,17276,17277,17278,17279,17280,17281,
        17282,17283,17284,17285,17286,17287,17288,17289,17290,17291,17292,17293,17294,
        17295,17525,17526,17527,17528,17529,17530,17531,17532,17533,17534,17535,17536,
        17537,17538,17539,17540,17541,17542,17543,17544,17545,17546,17547,17548,17549,
        17550,17551]

reduction_manager = PropertyManager()
PropertyManagerDataServiceImpl.Instance().addOrReplace("reduction_manager", reduction_manager)
reduction_manager.declareProperty("LoadAlgorithm", '{"name":"LoadILLSANS","parameters":{"OutputWorkspace":"ws"}}')

path = "/users/bush/mantid_data/D33_Comparison_TOF_mono/Data/rawdata/"

beam_x, beam_y, message = SANSBeamFinder(path + "002192.nxs", ReductionProperties="reduction_manager")
print(beam_x, beam_y, message)
print reduction_manager.getPropertyValue("LoadAlgorithm")
dark_current_subtracted_ws, dark_current_ws, dark_current_message = EQSANSDarkCurrentSubtraction(sample_ws, path + "002227.nxs",  ReductionProperties="reduction_manager")
#EQSANSNormalise - this attempts to do something with beam current
SANSMask(Workspace=dark_current_subtracted_ws, MaskedDetectorList=maskedDetectors)
solid_angle_corrected_ws, solid_angle_message = SANSSolidAngleCorrection(InputWorkspace = dark_current_subtracted_ws, ReductionProperties="reduction_manager")
sensitivity_corrected_ws, sensitivity_ws, sensitivity_message = SANSSensitivityCorrection(InputWorkspace= solid_angle_corrected_ws, Filename=path + "002229.nxs", ReductionProperties="reduction_manager")
#direct_beam_corrected_ws, transmission_ws, raw_transmission_ws, direct_beam_message = EQSANSDirectBeamTransmission(InputWorkspace=sensitivity_corrected_ws, SampleDataFilename=path + '002197.nxs', EmptyDataFilename=path + '002192.nxs', ReductionProperties='reduction_manager')
scaled_ws, scaled_message = SANSAbsoluteScale(InputWorkspace=sensitivity_corrected_ws, ReductionProperties='reduction_manager')
normalised_ws, normalised_message = NormaliseByThickness(InputWorkspace=scaled_ws, SampleThickness=0.1)
reducded_ws, reducded_message = EQSANSAzimuthalAverage1D(InputWorkspace=normalised_ws, ReductionProperties='reduction_manager')
#EQSANS2D
