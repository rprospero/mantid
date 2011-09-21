#include "MantidMDAlgorithms/Load_MDWorkspace.h"
#include <Poco/Path.h>

namespace Mantid{
    namespace MDAlgorithms{
        using namespace Mantid;
        using namespace MDDataObjects;
        using namespace Kernel;
        using namespace API;
        using namespace Geometry;


// Register the class into the algorithm factory
DECLARE_ALGORITHM(Load_MDWorkspace)
// logger for loading workspaces  
Kernel::Logger& Load_MDWorkspace::ldmdws_log =Kernel::Logger::get("MD-Algorithms");


Load_MDWorkspace::Load_MDWorkspace(void): 
API::Algorithm(),
test_mode(false)
{}

/** Destructor     */
Load_MDWorkspace::~Load_MDWorkspace()
{
 
}
//
void
Load_MDWorkspace::init()
{
    std::vector<std::string> ext(1);
    ext[0]=".sqw";
    declareProperty(new API::FileProperty("inFilename","", API::FileProperty::Load,ext), "The file containing initial MD dataset");
    declareProperty(new WorkspaceProperty<MDWorkspace>("MDWorkspace","",Direction::Output),"final MD workspace");
    // prospective property TODO: implement
    declareProperty((new Kernel::PropertyWithValue<bool>("LoadPixels",false,Direction::InOut)),
        "This property specifies if user wants to try loading in memory"
        " all pixels(events) availible in the MD workspace; If there is enough memory system will try to do it but may fail");
}
//
void 
Load_MDWorkspace::set_test_mode(const Geometry::MDGeometryDescription &testDatasetDescription)
{
    test_mode = true;
    pTestGeometryDescription = std::auto_ptr<Geometry::MDGeometryDescription>(new Geometry::MDGeometryDescription(testDatasetDescription));

}
void 
Load_MDWorkspace::exec()
{

MDWorkspace_sptr inputWS;
std::string workspaceFileName;

  // get the workspace and overwrite it if it already exists
   inputWS = this->getProperty("MDWorkspace");
   if(inputWS){
       ldmdws_log.information()<<" Existing workspace will be overwtitten\n";
   }else{
       inputWS = MDWorkspace_sptr(new MDWorkspace());
       AnalysisDataService::Instance().addOrReplace("MDWorkspace", inputWS);
   }
   this->setProperty("MDWorkspace",inputWS);

   std::auto_ptr<IMD_FileFormat> pFileReader;
   if(test_mode){
        pFileReader = MD_FileFormatFactory::getFileReader(workspaceFileName.c_str(),test_data,pTestGeometryDescription.get());
   }else{
        if(existsProperty("inFilename")){
            workspaceFileName = this->getPropertyValue("inFilename");
            workspaceFileName = Poco::Path(Poco::Path::current()).resolve(workspaceFileName).toString();
        }

        pFileReader = MD_FileFormatFactory::getFileReader(workspaceFileName.c_str());
   }
   if(!pFileReader.get()){
         ldmdws_log.error()<<" can not obtain file reader for MD file";
         throw(Kernel::Exception::FileError("can not get proper file reader",workspaceFileName));
   }
   std::auto_ptr<MDGeometryBasis> pBasis = std::auto_ptr<MDGeometryBasis>(new Geometry::MDGeometryBasis());
   
   pFileReader->read_basis(*pBasis);

   MDGeometryDescription geomDescr(pBasis->getNumDims(),pBasis->getNumReciprocalDims());

    // read the geometry description
   pFileReader->read_MDGeomDescription(geomDescr);

    // obtain the MDPoint description now (and MDPointsDescription in a future)
    MDPointDescription pd = pFileReader->read_pointDescriptions();

    // workspace now takes control for the file reader; initial pointer becomes invalid
    // this function will read MDImage and initiate MDDataPoints accordingly
    inputWS->init(pFileReader,pBasis,geomDescr,pd);

    ldmdws_log.information()<<" successfully loaded residental parts of the MDWorkspace: \""<<inputWS->getName()
                            <<"\" for file \""<<workspaceFileName<<"\"in memory\n";

    bool loadPix =  this->getProperty("LoadPixels");
    if(loadPix){
        IMD_FileFormat& Reader = inputWS->get_const_FileReader();	
        bool load_success=Reader.read_pix(*(inputWS->get_spMDDPoints()),true);

        if(load_success){
            ldmdws_log.information()<<" successfully loaded "<<inputWS->get_spMDDPoints()->getNumPixels()<<" pixels in memory\n";
        }else{
            ldmdws_log.information()<<" can not load "<<inputWS->get_spMDDPoints()->getNumPixels()<<" pixels in memory\n";
            this->setPropertyValue("LoadPixels","0");
        }
    }
    // clear test mode if it was eventually set-up above
    test_mode=false;
    pTestGeometryDescription.reset();
}
} // end namespaces
}
