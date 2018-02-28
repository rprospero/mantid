//
// Created by michael on 23/08/17.
//

#ifndef MANTID_NEXUS_GEOMETRY_PARSER_H_
#define MANTID_NEXUS_GEOMETRY_PARSER_H_

#include "MantidNexusGeometry/ParsingErrors.h"
// All possible derived classes from InstrumentAbstractBuilder
#include "MantidNexusGeometry/InstrumentGeometryAbstraction.h"
// All possible derived classes from ShapeAbstractCreator
#include "MantidNexusGeometry/ShapeGeometryAbstraction.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <H5Cpp.h>

#include <memory>
#include <vector>

namespace Mantid {
namespace NexusGeometry {

// Choose which derived instrumentAbstraction to use
typedef InstrumentGeometryAbstraction iAbstractBuilder;
typedef std::shared_ptr<iAbstractBuilder> iAbstractBuilder_sptr;
typedef ShapeGeometryAbstraction shapeAbsCreator;

class DLLExport NexusGeometryParser {
public:
  /// Constructor // , std::weak_ptr<InstrumentHandler> iHandler
  explicit NexusGeometryParser(const H5std_string &fileName,
                               iAbstractBuilder_sptr iAbsBuilder_sptr);

  /// Destructor
  ~NexusGeometryParser() = default;

  /// OFF INSTRUMENT GEOMETRY PARSER - returns exit status to LoadNexusGeometry
  ParsingErrors parseNexusGeometry();

private:
  shapeAbsCreator sAbsCreator = shapeAbsCreator();
  H5::H5File nexusFile;
  H5::Group rootGroup;
  ParsingErrors exitStatus = NO_ERROR;
  /// Instrument abstraction builder
  iAbstractBuilder_sptr iBuilder_sptr;
  /// Opens sub groups of current group
  std::vector<H5::Group> openSubGroups(H5::Group &parentGroup,
                                       const H5std_string &CLASS_TYPE);
  /// Opens all detector groups in a file
  std::vector<H5::Group> openDetectorGroups();
  /// Stores detectorGroup detIds in vector of ints
  std::vector<int> getDetectorIds(H5::Group &detectorGroup);
  /// Stores detectorGroup pixel offsets as Eigen 3xN matrix
  Eigen::Matrix<double, 3, Eigen::Dynamic>
  getPixelOffsets(H5::Group &detectorGroup);
  /// Gets the transformations applied to the detector's pixelOffsets
  Eigen::Transform<double, 3, Eigen::Affine>
  getTransformations(const H5::Group &detectorGroup);
  /// Gets the data from a string dataset
  H5std_string get1DStringDataset(const H5std_string &dataset,
                                  const H5::Group &group);
  /// Read dataset into vector
  template <typename valueType>
  std::vector<valueType> get1DDataset(const H5std_string &dataset);
  template <typename valueType>
  std::vector<valueType> get1DDataset(const H5std_string &dataset,
                                      const H5::Group &group);
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
  parsePixelShape(const H5::Group &detectorGroup);
  /// Parse shape - choose what type shape
  objectHolder parseNexusShape(const H5::Group &detectorGroup);
  /// Parse cylinder nexus geometry
  objectHolder parseNexusCylinder(const H5::Group &shapeGroup);
  /// Parse OFF (mesh) nexus geometry
  objectHolder parseNexusMesh(const H5::Group &shapeGroup);
  /// Parse source
  void parseAndAddSource();
  /// Parse sample
  void parseAndAddSample();
  /// Parse monitors
  void parseMonitors();

  /// From the NeXus-OFF polygonal mesh, create a triangular mesh
  std::vector<uint16_t>
  createTriangularFaces(const std::vector<uint16_t> &faceIndices,
                        const std::vector<uint16_t> &windingOrder) const;

  void createTrianglesFromPolygon(const std::vector<uint16_t> &windingOrder,
                                  std::vector<uint16_t> &triangularFaces,
                                  int &startOfFace, int &endOfFace) const;
};

} // namespace NexusGeometry
} // namespace Mantid

#endif // MANTID_NEXUS_GEOMETRY_PARSER_H_
