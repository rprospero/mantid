#ifndef H_CONV2_MDEVENTS_DET_INFO
#define H_CONV2_MDEVENTS_DET_INFO
/** This structure is the basis and temporary replacement for future subalgorithm, which calculates 
   * matrix worrkspace with various precprocessed detectors parameters
   * 
   * @date 22-12-2011

    Copyright &copy; 2010 ISIS Rutherford Appleton Laboratory & NScD Oak Ridge National Laboratory

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

        File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
        Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
#include "MantidKernel/System.h"
#include "MantidKernel/PhysicalConstants.h"
#include "MantidKernel/V3D.h"
#include "MantidDataObjects/Workspace2D.h"
  /** the lightweight class below contain 3D uint vectors, pointing to the positions of the detectors
      This vector used to preprocess and catch the partial positions of the detectors in Q-space
      to avoid repetative calculations, and (possibly) to write these data as part of the physical compression scheme
      in a very common situation when the physical instrument does not change in all runs, contributed into MD workspace
   */
namespace Mantid
{
namespace MDAlgorithms
{

struct preprocessed_detectors{
    double L1;
    Kernel::V3D   minDetPosition;    // minimal and
    Kernel::V3D   maxDetPosition;    // maxinal position for the detectors
    std::vector<Kernel::V3D>  det_dir; // unit vector pointing from the sample to the detector;
    std::vector<double>       L2;
    std::vector<double>       TwoTheta;
    std::vector<int32_t>      det_id;   // the detector ID;
    std::vector<size_t>       detIDMap;
    //
    bool is_defined(void)const{return det_dir.size()>0;}
    bool is_defined(size_t new_size)const{return det_dir.size()==new_size;}
    double  *  pL2(){return &L2[0];}
    double  *  pTwoTheta(){return &TwoTheta[0];}
    size_t  *  iDetIDMap(){return &detIDMap[0];}
    Kernel::V3D  * pDetDir(){return &det_dir[0];}
};

/** helper function, does preliminary calculations of the detectors positions to convert results into k-dE space ;
      and places the resutls into static cash to be used in subsequent calls to this algorithm */
void DLLExport processDetectorsPositions(const DataObjects::Workspace2D_const_sptr inputWS,preprocessed_detectors &det,Kernel::Logger& convert_log);
} // end MDAlgorithms
}
#endif