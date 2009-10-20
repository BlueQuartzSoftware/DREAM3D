///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _MXAFileSystemPath_h_
#define _MXAFileSystemPath_h_

#include <MXA/Common/MXATypes.h>


//-- STL includes
#include <string>


/**
* @class MXAFileSystemPath MXAFileSystemPath.h PathToHeader/MXAFileSystemPath.h
* @brief The class allows for some interrogation of the file system regarding the
* existance of files or directories and also allows for the creation and deletion
* of files or directories from the filesystem.
*
* @author mjackson
* @date Mar 10, 2009
* @version $Revision$
*/
class MXAFileSystemPath
{
  public:
#if defined (WIN32)
    static MXA_EXPORT const char Separator = '\\';
#else
    static MXA_EXPORT const char Separator = '/';
#endif
    static MXA_EXPORT const char UnixSeparator = '/';
    static MXA_EXPORT const char Dot = '.';


    static MXA_EXPORT std::string getSeparator();
    /**
     * @brief Is the path specified a directory on the filesystem
     * @param path Path to examine
     */
    static MXA_EXPORT bool isDirectory(const std::string &path);

    /**
     * @brief Does the path designate a file on the file system
     * @param path Path to examine
     */
    static MXA_EXPORT bool isFile(const std::string &path);

    /**
     * @brief Returns true if the path is a relative path but does not determine
     * if the file actually exists or not
     * @@param path The path to check
     * @return True if the path is relative
     */
    static MXA_EXPORT bool isRelativePath(const std::string &path);

    /**
     * @brief Returns true if the path is an absolute path. On Unix this means the
     * first character is '/' and on windows the path starts with 'Drive:\' or '\\'
     * @@param path The path to check
     * @return True if the path is absolute
     */
    static MXA_EXPORT bool isAbsolutePath(const std::string &path);

    /**
     * @brief Returns the current path
     * @return The current working directory as reported by the operating system
     */
    static MXA_EXPORT std::string currentPath();

    /**
     * @brief Returns the path to the parent directory
     * @param path The path to return the parent path
     * @return The Parent path
     */
    static MXA_EXPORT std::string parentPath(const std::string &path);


    /**
     * @brief Either calculates an absolute path or returns the same string if
     * it already indicates an absolute path. No Attempt is made to actually
     * determine if the file exists or not. The path will be free of any extra
     * './' or '..' in the path but symbolic links will possibly be in the path
     * @param path The path to check/convert
     * @return The absolute path.
     */
    static MXA_EXPORT std::string absolutePath(const std::string &path);

    /**
     * @brief Returns a list of the contents of a directory. No filtering is attempted.
     * @param path The path to the directory
     * @return List of contents
     */
    static MXA_EXPORT std::vector<std::string> entryList(const std::string &path);

    /**
     * @brief Get the size of the file in bytes
     */
    static MXA_EXPORT uint64 fileSize(const std::string &path);

    /**
     * @brief Does the path actually exist on the file system
     * @param path Path to examine
     */
    static MXA_EXPORT bool exists(const std::string &path);

    /**
     * @brief Get any file extension on the filepath
     * @param path Path to examine
     */
    static MXA_EXPORT std::string extension(const std::string &path);

    /**
     * @brief Return the filename WITH the extension
     * @param path Path to examine
     */
    static MXA_EXPORT std::string filename(const std::string &path);

    /**
     * @brief Returns the filename without the extension
     * @param path Path to examine
     */
    static MXA_EXPORT std::string fileNameWithOutExtension(const std::string &path);

    /**
     * @brief Create a directory or structure of directories
     * @param path The path to create
     * @param createParentDirectories If true then any directories missing from
     * the path will also be created.
     * @return True if all directories were created successfully.
     */
    static MXA_EXPORT bool mkdir(const std::string &path, bool createParentDirectories);

    /**
     * @brief Removes a directory from the file system. Note that the directory
     * must be empty, including hidden files
     * @param path to delete from the filesystem
     * @param recurseParentDirectories
     */
    static MXA_EXPORT bool rmdir(const std::string &path, bool recurseParentDirectories);

    /**
     * @brief Remove a file from the filesystem
     * @param path The path to the file to remove
     * @return True on successful removal
     */
    static MXA_EXPORT bool remove(const std::string &path);

    /**
     * @brief Cleans a file system path of extra './', '//' and '/../' elements
     * @param path Path to clean
     * @return A new string containing the cleaned path.
     */
    static MXA_EXPORT std::string cleanPath(const std::string &path);

    /**
     * @brief Converts from native directory separators to unix separators
     * @param path The path to conver
     * @return Newly converted path
     */
    static MXA_EXPORT std::string fromNativeSeparators(const std::string  &path);

    /**
     * @brief Converts a path to use native directory separators
     * @param path The path to convert
     * @return The newly converted path
     */
    static MXA_EXPORT std::string toNativeSeparators(const std::string &path);

#if defined (WIN32)
    static MXA_EXPORT bool isDirPath(const std::string &path, bool *existed);
#endif

  protected:
    MXAFileSystemPath();
    ~MXAFileSystemPath();

  private:
      MXAFileSystemPath(const MXAFileSystemPath&);    //Not Implemented
      void operator=(const MXAFileSystemPath&); //Not Implemented

};

#endif //_MXAFileSystemPath_h_



