#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

#include <QtCore/QString>

#define FILTER_PARAMETER_COMPATIBILITY_CHECK(FILTER_PTR, FP_CLASS, WIDGET_CLASS)                                                                                                                       \
  FP_CLASS* filter_parameter_check = dynamic_cast<FP_CLASS*>(parameter);                                                                                                                               \
  if(nullptr == filter_parameter_check)                                                                                                                                                                \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    ss << #WIDGET_CLASS << " can ONLY be used with " << #FP_CLASS << " objects. The programmer of the filter has a bug.";                                                                              \
    ss << " The name of the filter was " << FILTER_PTR->getHumanLabel().toStdString() << " and the name of the Filter Parameter was " << parameter->getHumanLabel().toStdString();                     \
    ss << " and is trying to get the propery " << parameter->getPropertyName().toStdString() << " in the filter";                                                                                      \
    throw std::invalid_argument(ss.str());                                                                                                                                                             \
  }

#define FILTER_COMPATIBILITY_CHECK(FILTER_CLASS, WIDGET_CLASS)                                                                                                                                         \
  FILTER_CLASS* filter_check = dynamic_cast<FILTER_CLASS*>(filter);                                                                                                                                    \
  if(nullptr == filter_check)                                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    std::stringstream ss;                                                                                                                                                                              \
    ss << #WIDGET_CLASS << " can ONLY be used with " << #FILTER_CLASS << " objects. The programmer of the filter has a bug.";                                                                          \
    ss << " The name of the filter was " << filter->getHumanLabel().toStdString() << " and the name of the Filter Parameter was " << parameter->getHumanLabel().toStdString();                         \
    ss << " and is trying to get the propery " << parameter->getPropertyName().toStdString() << " in the filter";                                                                                      \
    throw std::invalid_argument(ss.str());                                                                                                                                                             \
  }

/**
 * @namespace OrientationAnalysis
 * @brief These are strings that the plugin uses for display in error and other information messages.
 */
namespace OrientationAnalysisConstants
{
const QString OrientationAnalysisPluginFile("OrientationAnalysisPlugin");
const QString OrientationAnalysisPluginDisplayName("OrientationAnalysis");
const QString OrientationAnalysisBaseName("OrientationAnalysis");
namespace BlueQuartz
{
const QString VendorName("BlueQuartz Software, LLC");
const QString URL("http://www.bluequartz.net");
const QString Copyright("(C) 2018 BlueQuartz Software, LLC");
} // namespace BlueQuartz

namespace
{
static const int AnorthicType = 0; // Triclinic
static const int CyclicType = 1;
static const int DihedralType = 2;
static const int TetrahedralType = 3;
static const int OctahedralType = 4;
} // namespace

namespace
{
static const int NoAxisOrder = 0;
static const int TwoFoldAxisOrder = 2;
static const int ThreeFoldAxisOrder = 3;
static const int FourFoldAxisOrder = 4;
static const int SixFoldAxisOrder = 6;
} // namespace

static const int FZtarray[32] = {AnorthicType, AnorthicType, CyclicType,   CyclicType,   CyclicType,   DihedralType,    DihedralType,    DihedralType,   CyclicType,      CyclicType,    CyclicType,
                                 DihedralType, DihedralType, DihedralType, DihedralType, CyclicType,   CyclicType,      DihedralType,    DihedralType,   DihedralType,    CyclicType,    CyclicType,
                                 CyclicType,   DihedralType, DihedralType, DihedralType, DihedralType, TetrahedralType, TetrahedralType, OctahedralType, TetrahedralType, OctahedralType};

static const int FZoarray[32] = {NoAxisOrder,        NoAxisOrder,        TwoFoldAxisOrder,   TwoFoldAxisOrder,   TwoFoldAxisOrder,  TwoFoldAxisOrder,  TwoFoldAxisOrder,  TwoFoldAxisOrder,
                                 FourFoldAxisOrder,  FourFoldAxisOrder,  FourFoldAxisOrder,  FourFoldAxisOrder,  FourFoldAxisOrder, FourFoldAxisOrder, FourFoldAxisOrder, ThreeFoldAxisOrder,
                                 ThreeFoldAxisOrder, ThreeFoldAxisOrder, ThreeFoldAxisOrder, ThreeFoldAxisOrder, SixFoldAxisOrder,  SixFoldAxisOrder,  SixFoldAxisOrder,  SixFoldAxisOrder,
                                 SixFoldAxisOrder,   SixFoldAxisOrder,   SixFoldAxisOrder,   NoAxisOrder,        NoAxisOrder,       NoAxisOrder,       NoAxisOrder,       NoAxisOrder};
} // namespace OrientationAnalysisConstants

/**
 * @namespace FilterParameterWidgetType
 * @brief If you create custom Filter Parameter Widgets for your classes then those need to be defined here
 */
namespace FilterParameterWidgetType
{

const QString EbsdToH5EbsdWidget("EbsdToH5EbsdWidget");
const QString ReadH5EbsdWidget("ReadH5EbsdWidget");
const QString ConvertHexGridToSquareGridWidget("ConvertHexGridToSquareGridWidget");
} // namespace FilterParameterWidgetType
