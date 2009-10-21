/* <distribution-statement>
 * *****************************************************************************
 * Distribution authorized to DOD components and DOD contractors only;         *
 * Administrative or Operational Use;                                          *
 * February 2009.                                                              *
 * Other requests for this document shall be referred to                       *
 * U.S. Air Force Research Laboratory                                          *
 * 2230 Tenth St., Ste. 1                                                      *
 * Wright-Patterson AFB, OH 45433-7817                                         *
 *                                                                             *
 * Destroy by contents or reconstruction of the document.                      *
 *                                                                             *
 * This code was written under the U.S. Air Force contrct F33615-03-C-5220     *
 * *****************************************************************************
 * </distribution-statement> */

#ifndef ZEISSDIRECTORYPATTERNS_H_
#define ZEISSDIRECTORYPATTERNS_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <string>

namespace Ang {
  namespace DirectoryPatterns {
    const std::string Dot (".");
  }
}


/**
 *
 */
class AngDirectoryPatterns
{
  public:
    MXA_SHARED_POINTERS(AngDirectoryPatterns)
    MXA_TYPE_MACRO(AngDirectoryPatterns)
    MXA_STATIC_NEW_MACRO(AngDirectoryPatterns)


    virtual ~AngDirectoryPatterns();

    MXA_INSTANCE_STRING_PROPERTY(ParentDirectory, m_ParentDirectory)
    MXA_INSTANCE_STRING_PROPERTY(Prefix, m_Prefix)
    MXA_INSTANCE_STRING_PROPERTY(Suffix, m_Suffix)
    MXA_INSTANCE_STRING_PROPERTY(Extension, m_Extension)

    std::string generateFullPathAngFileName(int slice, int numberWidth);

    std::string generateAngFileName(int slice, int numberWidth);

    int parseDocumentName(const std::string &guessedPrefix,
                                  const std::string &zeissDocumentName);

    int extractSlice(const std::string &directoryName);

    void print(std::ostream &ostream);

protected:
    AngDirectoryPatterns();

  private:


    AngDirectoryPatterns(const AngDirectoryPatterns&);    // Copy Constructor Not Implemented
    void operator=(const AngDirectoryPatterns&);  // Operator '=' Not Implemented
};

#endif /* ZEISSDIRECTORYPATTERNS_H_ */
