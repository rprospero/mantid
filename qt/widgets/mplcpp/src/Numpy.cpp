#include "MantidQtWidgets/MplCpp/Numpy.h"

// See https://docs.scipy.org/doc/numpy/reference/c-api.array.html#miscellaneous
#define PY_ARRAY_UNIQUE_SYMBOL MPLCPP_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#include <algorithm>
#include <type_traits>

namespace {
// Simply struct to aid in global initialization of numpy
struct ImportArray {
  ImportArray() {
    Py_Initialize();
    import_array();
  }
  ~ImportArray() { Py_Finalize(); }
};
// static instance to call import_array()
// Do not remove this!!
ImportArray _importer;
}

namespace MantidQt {
namespace Widgets {
namespace MplCpp {

template <typename Iterable> PythonObject copyToNDArray(const Iterable &data) {
  static_assert(std::is_same<typename Iterable::value_type, double>::value,
                "Element type must be double.");
  npy_intp length = static_cast<npy_intp>(data.size());
  auto ndarray =
      PyArray_New(&PyArray_Type, 1, &length, NPY_DOUBLE, nullptr, nullptr,
                  sizeof(npy_double), NPY_ARRAY_CARRAY, nullptr);
  auto emptyData =
      static_cast<double *>(PyArray_DATA((PyArrayObject *)ndarray));
  std::copy(std::begin(data), std::end(data), emptyData);
  return PythonObject(NewRef(ndarray));
}

// Explicit instantiations
template EXPORT_OPT_MANTIDQT_MPLCPP PythonObject
copyToNDArray<std::vector<double>>(const std::vector<double> &);
}
}
}
