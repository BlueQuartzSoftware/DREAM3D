#ifndef _orientationanalysisconstants_h_
#define _orientationanalysisconstants_h_

#include <QtCore/QString>
/**
 * @namespace OrientationAnalysis
 * @brief These are strings that the plugin uses for display in error and other information messages.
 */
namespace OrientationAnalysisConstants
{
  const QString OrientationAnalysisPluginFile("OrientationAnalysisPlugin");
  const QString OrientationAnalysisPluginDisplayName("OrientationAnalysis");
  const QString OrientationAnalysisBaseName("OrientationAnalysis");

  namespace
  {
  static const int AnorthicType = 0; // Triclinic
  static const int CyclicType = 1;
  static const int DihedralType = 2;
  static const int TetrahedralType = 3;
  static const int OctahedralType = 4;
  }

  namespace
  {
  static const int NoAxisOrder = 0;
  static const int TwoFoldAxisOrder = 2;
  static const int ThreeFoldAxisOrder = 3;
  static const int FourFoldAxisOrder = 4;
  static const int SixFoldAxisOrder = 6;
  }

  static const int FZtarray[32] = {AnorthicType, AnorthicType, CyclicType,   CyclicType,   CyclicType,   DihedralType,    DihedralType,    DihedralType,   CyclicType,      CyclicType,    CyclicType,
                                   DihedralType, DihedralType, DihedralType, DihedralType, CyclicType,   CyclicType,      DihedralType,    DihedralType,   DihedralType,    CyclicType,    CyclicType,
                                   CyclicType,   DihedralType, DihedralType, DihedralType, DihedralType, TetrahedralType, TetrahedralType, OctahedralType, TetrahedralType, OctahedralType};

  static const int FZoarray[32] = {NoAxisOrder,        NoAxisOrder,        TwoFoldAxisOrder,   TwoFoldAxisOrder,   TwoFoldAxisOrder,  TwoFoldAxisOrder,  TwoFoldAxisOrder,  TwoFoldAxisOrder,
                                   FourFoldAxisOrder,  FourFoldAxisOrder,  FourFoldAxisOrder,  FourFoldAxisOrder,  FourFoldAxisOrder, FourFoldAxisOrder, FourFoldAxisOrder, ThreeFoldAxisOrder,
                                   ThreeFoldAxisOrder, ThreeFoldAxisOrder, ThreeFoldAxisOrder, ThreeFoldAxisOrder, SixFoldAxisOrder,  SixFoldAxisOrder,  SixFoldAxisOrder,  SixFoldAxisOrder,
                                   SixFoldAxisOrder,   SixFoldAxisOrder,   SixFoldAxisOrder,   NoAxisOrder,        NoAxisOrder,       NoAxisOrder,       NoAxisOrder,       NoAxisOrder};
}

/**
 * @namespace FilterParameterWidgetType
 * @brief If you create custom Filter Parameter Widgets for your classes then those need to be defined here
 */
namespace FilterParameterWidgetType
{

  const QString EbsdToH5EbsdWidget("EbsdToH5EbsdWidget");
  const QString ReadH5EbsdWidget("ReadH5EbsdWidget");
  const QString ConvertHexGridToSquareGridWidget("ConvertHexGridToSquareGridWidget");
}
namespace BlueQuartz
{
  const QString VendorName("BlueQuartz Software, LLC");
  const QString URL("http://www.bluequartz.net");
  const QString Copyright("(C) 2016 BlueQuartz Software, LLC");
}

#endif
