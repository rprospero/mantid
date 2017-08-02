==========================
Indirect Inelastic Changes
==========================

.. contents:: Table of Contents
   :local:

New features
------------

Algorithms
##########

- :ref:`SimpleShapeMonteCarloAbsorption <algm-SimpleShapeMonteCarloAbsorption>` has been added to simplify sample environment inputs for MonteCarloAbsorption

Data Analysis
#############

Jump Fit
~~~~~~~~

Improvements
------------
- The *S(Q, W)* interface now automatically replaces NaN values with 0.


Bugfixes
--------
- The number of vanadium and sample files no longer need to be the same when running
  ISISIndirectDiffractionReduction, when the SumFiles property is set to true.
- If 'Use Vanadium File' is checked, it is now necessary to supply at least one vanadium
  file.
- ElasticWindowMultiple now correctly normalizes by the lowest temperature - rather than the first one.
- An issue has been fixed in :ref:`algm-IndirectILLEnergyTransfer` when handling the data with mirror sense, that have shifted 0 monitor counts in the left and right wings. This was causing the left and right workspaces to have different x-axis binning and to fail to sum during the unmirroring step. 
- An issue has been fixed in :ref:`algm-IndirectILLReductionFWS` when the scaling of the data after vanadium calibration was not applied.

`Full list of changes on GitHub <http://github.com/mantidproject/mantid/pulls?q=is%3Apr+milestone%3A%22Release+3.11%22+is%3Amerged+label%3A%22Component%3A+Indirect+Inelastic%22>`_
