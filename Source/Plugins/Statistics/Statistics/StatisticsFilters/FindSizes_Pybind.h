#ifndef pybind_FindSizes_H_
#define pybind_FindSizes_H_

/**
* @brief This header file is genererate from a program. Make changes with caution.
* 
* This header file wraps the SIMPL class FindSizes located at
* Statistics/StatisticsFilters/FindSizes.h. 
* The Python bindings are created using Pybind11.
*/

#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "Statistics/StatisticsFilters/FindSizes.h"

/**
* @brief This defines a C++11 alias so loading the file by itself into an IDE
* will allow the proper code completion for a wrapped std::shared_ptr<> class.
*/
#ifndef PySharedPtrClass_TEMPLATE
#define PySharedPtrClass_TEMPLATE
template <typename T>
using PySharedPtrClass = py::class_<T, std::shared_ptr<T> >;
#endif

/**
* @brief
* @param m The Python module
* @param parent The parent class of the wrapped class
* @return A std::shared_ptr<T> wrapped insance of the wrapped class properly 
* initialized.
*/
PySharedPtrClass<FindSizes> pybind11_init_Statistics_FindSizes(py::module &m, PySharedPtrClass<AbstractFilter>& parent)
{
  PySharedPtrClass<FindSizes> instance(m, "FindSizes", parent);
  instance
  .def(py::init([](){ return FindSizes::New();}))
  .def_static("New", &FindSizes::New)
  /* Property accessors for VolumesArrayName */
  .def("getVolumesArrayName", &FindSizes::getVolumesArrayName)
  .def("setVolumesArrayName", &FindSizes::setVolumesArrayName, py::arg("VolumesArrayName"))
  /* Property accessors for EquivalentDiametersArrayName */
  .def("getEquivalentDiametersArrayName", &FindSizes::getEquivalentDiametersArrayName)
  .def("setEquivalentDiametersArrayName", &FindSizes::setEquivalentDiametersArrayName, py::arg("EquivalentDiametersArrayName"))
  /* Property accessors for NumElementsArrayName */
  .def("getNumElementsArrayName", &FindSizes::getNumElementsArrayName)
  .def("setNumElementsArrayName", &FindSizes::setNumElementsArrayName, py::arg("NumElementsArrayName"))
  ;
  return instance;
}


#endif /* pybind_FindSizes_H_  */
