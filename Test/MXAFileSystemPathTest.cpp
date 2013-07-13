///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#include "MXA/MXA.h"
#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXADir.h>
#include <MXA/Utilities/MXAFileInfo.h>

#include "TestFileLocations.h"
#include "UnitTestSupport.hpp"

#include <iostream>
#include <fstream>

#include <boost/shared_ptr.hpp>


#define PRINT_LINE_NUMBER() \
  std::cout << "|--MXADirTest.cpp(" << __LINE__ << ")" << std::endl;

#define FileWriter MXAFILEWRITER_CLASS_NAME
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
  std::cout << "|--Removing Test files" << std::endl;
#if REMOVE_TEST_FILES
  MXADir::remove(MXAUnitTest::MXAFileSystemPathTest::OutputFile);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CheckFile(const std::string &filepath,
               const std::string &filename,
               const std::string &extension)
{
  std::cout  << "|-- CheckFile " << filepath << std::endl;
  bool exists;
  bool isDir;
  bool isFile;
  bool ok;
    {
      // Write a file so we can try and delete it
      std::cout << "|--  Create: '" << filepath << "'" << std::endl;
      std::ofstream outStream(filepath.c_str(), std::ios::out | std::ios::binary);
      DREAM3D_REQUIRE_EQUAL(false, outStream.fail() );
      std::string data ( "MXADir_Test Contents");
      outStream.write(data.c_str(), data.length());
      DREAM3D_REQUIRE_EQUAL (outStream.bad(), false);
      outStream.close();
    }


    std::string compName = filename;
    if (compName.at(compName.size()-1) == '.')
    {
      compName = compName.substr(0, compName.size() - 1);
    }

    isDir = MXADir::isDirectory(filepath);
    DREAM3D_REQUIRE_EQUAL(isDir, false);
    isFile = MXADir::isFile(filepath);
    DREAM3D_REQUIRE_EQUAL(isFile, true);
    exists = MXADir::exists(filepath);
    DREAM3D_REQUIRE_EQUAL(exists, true);
    std::string fn = MXAFileInfo::filename(filepath);
    DREAM3D_REQUIRE_EQUAL(fn, compName);
    std::string ext = MXAFileInfo::extension(filepath);
    DREAM3D_REQUIRE_EQUAL(ext, extension);

    // Now try to delete the file
    std::cout << "|--  Delete: '" << filepath << "'" << std::endl;
    ok = MXADir::remove(filepath);
    DREAM3D_REQUIRE_EQUAL(ok, true);
    exists = MXADir::exists(filepath);
    DREAM3D_REQUIRE_EQUAL(exists, false);

    std::cout << "|--  Delete Again:" << std::endl;
    ok = MXADir::remove(filepath);
    DREAM3D_REQUIRE_EQUAL(ok, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileNameTest()
{
  int err = 0;

  std::string filename = "some.thing";
  std::string filepath = filename;
  std::string ext = "thing";
  CheckFile(filepath, filename, ext);

  filename = ".some.thing";
  filepath = filename;
  ext = "thing";
  CheckFile(filepath, filename, ext);

  filename = ".something";
  filepath = filename;
  ext = "";
  CheckFile(filepath, filename, ext);

  filename = "something";
  filepath = filename;
  ext = "";
  CheckFile(filepath, filename, ext);

  filename = "something.";
  filepath = filename;
  ext = "";
  CheckFile(filepath, filename, ext);

  filename = ".some.thing.";
  filepath = filename;
  ext = "";
  CheckFile(filepath, filename, ext);


//------------------------------------------------------
#if defined (WIN32)
    const std::string DirSeparator = "\\";
#else
    const std::string DirSeparator = "/";
#endif
  filename = "some.thing";
  filepath = MXAUnitTest::MXATempDir + filename;
  ext = "thing";
  CheckFile(filepath, filename, ext);

  filename = ".some.thing";
  filepath = MXAUnitTest::MXATempDir + filename;
  ext = "thing";
  CheckFile(filepath, filename, ext);

  filename = ".something";
  filepath = MXAUnitTest::MXATempDir +  filename;
  ext = "";
  CheckFile(filepath, filename, ext);

  filename = "something";
  filepath = MXAUnitTest::MXATempDir + filename;
  ext = "";
  CheckFile(filepath, filename, ext);

  filename ="something.";
  filepath =  MXAUnitTest::MXATempDir + filename;
  ext = "";
  CheckFile(filepath, filename, ext);

  filename = ".some.thing.";
  filepath = MXAUnitTest::MXATempDir + filename;
  ext = "";
  CheckFile(filepath, filename, ext);

  bool exists = MXADir::exists(std::string(""));
  DREAM3D_REQUIRE_EQUAL(exists, false);

  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FilesTest()
{
  std::cout  << "|- FilesTest -----------------" << std::endl;
  int err = 0;
  bool ok;

  std::string testdir = MXAUnitTest::MXATempDir + MXAUnitTest::MXAFileSystemPathTest::TestDir;


  CheckFile(MXAUnitTest::MXAFileSystemPathTest::OutputFile,
                 MXAUnitTest::MXAFileSystemPathTest::OutputFileName,
                 MXAUnitTest::MXAFileSystemPathTest::Extension);

  std::string testFileName = ".hidden_file";
  std::string testFilePath = testdir + MXAUnitTest::DirSeparator + testFileName;
  std::string ext; // No Extension
  CheckFile(testFilePath, testFileName, ext);

  testFileName = "Normal.txt";
  ok = MXADir::mkdir(testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator, true);
  DREAM3D_REQUIRE_EQUAL(ok, true);
  testFilePath = testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator + testFileName;
  ext = "txt";
  CheckFile(testFilePath, testFileName, ext);
  ok = MXADir::rmdir(testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator, false);
  DREAM3D_REQUIRE_EQUAL(ok, true);

  testFileName = "No_Extension";
  ok = MXADir::mkdir(testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator, true);
  DREAM3D_REQUIRE_EQUAL(ok, true);
  testFilePath = testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator + testFileName;
  ext = "";
  CheckFile(testFilePath, testFileName, ext);
  ok = MXADir::rmdir(testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator, false);
  DREAM3D_REQUIRE_EQUAL(ok, true);

  testFileName = "EndsWithDot.";
  ok = MXADir::mkdir(testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator, true);
  DREAM3D_REQUIRE_EQUAL(ok, true);
  testFilePath = testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator + testFileName;
  ext = "";
  CheckFile(testFilePath, testFileName, ext);
  ok = MXADir::rmdir(testdir + MXAUnitTest::DirSeparator + "Dot.Dir" + MXAUnitTest::DirSeparator, false);
  DREAM3D_REQUIRE_EQUAL(ok, true);


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AbsolutePathTest()
{
  std::cout  << "|- AbsolutePathTest -----------------" << std::endl;
  int err = 0;
  int success = 0;

  std::string testdir = MXAUnitTest::MXATempDir + MXAUnitTest::MXAFileSystemPathTest::TestDir;

  std::string currentPath = MXADir::currentPath();
  std::string refPath = MXAUnitTest::MXATestBinaryDirectory;
  std::cout << "|++ currentPath: " << currentPath << std::endl;
  std::cout << "|++ refPath:     " << refPath << std::endl;
  success = currentPath.compare(refPath);
  DREAM3D_REQUIRE_EQUAL(success, 0);

  std::string file = MXAUnitTest::MXAFileSystemPathTest::OutputFileName;
  PRINT_LINE_NUMBER();
  CheckFile(MXAUnitTest::MXAFileSystemPathTest::OutputFileName, file, "bin");
  file = MXADir::absolutePath(file);
  refPath = MXAUnitTest::MXATestBinaryDirectory + MXADir::Separator + MXAUnitTest::MXAFileSystemPathTest::OutputFileName;
  std::cout << "|-- file:    " << file << std::endl;
  std::cout << "|-- refPath: " << refPath << std::endl;
  success = file.compare(refPath);
  DREAM3D_REQUIRE_EQUAL(success, 0);
  PRINT_LINE_NUMBER();
  CheckFile(  file, MXAUnitTest::MXAFileSystemPathTest::OutputFileName, "bin");

  // Check with a ./ prefixed to the file path
  file = MXAUnitTest::MXAFileSystemPathTest::OutputFileName;
  PRINT_LINE_NUMBER();
  CheckFile("." + MXADir::getSeparator() + MXAUnitTest::MXAFileSystemPathTest::OutputFileName, file, "bin");
  file = MXADir::absolutePath(file);
  refPath = MXAUnitTest::MXATestBinaryDirectory + MXADir::Separator + MXAUnitTest::MXAFileSystemPathTest::OutputFileName;
  std::cout << "|-- file:    " << file << std::endl;
  std::cout << "|-- refPath: " << refPath << std::endl;
  success = file.compare(refPath);
  DREAM3D_REQUIRE_EQUAL(success, 0);
  PRINT_LINE_NUMBER();
  CheckFile(file, MXAUnitTest::MXAFileSystemPathTest::OutputFileName, "bin");

  // Check with a ../ prefixed to the file name
  file = MXAUnitTest::MXAFileSystemPathTest::OutputFileName;
  PRINT_LINE_NUMBER();
  CheckFile( ".." + MXADir::getSeparator() + MXAUnitTest::MXAFileSystemPathTest::OutputFileName, file, "bin");
  file = MXAUnitTest::MXABuildDir + MXADir::Separator + file;
  refPath = MXAUnitTest::MXABuildDir + MXADir::Separator + MXAUnitTest::MXAFileSystemPathTest::OutputFileName;
  std::cout << "|-- file:    " << file << std::endl;
  std::cout << "|-- refPath: " << refPath << std::endl;
  success = file.compare(refPath);
  DREAM3D_REQUIRE_EQUAL(success, 0);
  PRINT_LINE_NUMBER();
  CheckFile(file, MXAUnitTest::MXAFileSystemPathTest::OutputFileName, "bin");


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MakeDirectoriesTest()
{

  std::cout  << "|- MakeDirectoriesTest -----------------" << std::endl;
    int err = 0;
  bool exists;
  bool isDir;
  bool isFile;
  bool ok;
  bool isRelative;
  bool isAbsolute;

  std::string testdir = MXAUnitTest::MXATempDir + MXAUnitTest::MXAFileSystemPathTest::TestDir;


  std::string dirPath( testdir
                      + MXADir::Separator
                      + MXAUnitTest::MXAFileSystemPathTest::TestDirName1
                      + MXADir::Separator);
  std::cout << "|-- Creating Deeply Nested Directories: '" << dirPath << "'" << std::endl;

  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, false);
  err = MXADir::mkdir(dirPath, true);
  DREAM3D_REQUIRE_EQUAL(err, 1);
  isDir = MXADir::isDirectory(dirPath);
  DREAM3D_REQUIRE_EQUAL(isDir, true);
  isFile = MXADir::isFile(dirPath);
  DREAM3D_REQUIRE_EQUAL(isFile, false);
  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, true);
  ok = MXADir::rmdir(dirPath, false);
  DREAM3D_REQUIRE_EQUAL(ok, true);
  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, false);

  dirPath = testdir
              + MXADir::Separator
              + MXAUnitTest::MXAFileSystemPathTest::TestDirName1;

  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, false);
  err = MXADir::mkdir(dirPath, true);
  DREAM3D_REQUIRE_EQUAL(err, 1);
  isDir = MXADir::isDirectory(dirPath);
  DREAM3D_REQUIRE_EQUAL(isDir, true);
  isFile = MXADir::isFile(dirPath);
  DREAM3D_REQUIRE_EQUAL(isFile, false);
  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, true);
  ok = MXADir::rmdir(dirPath, false);
  DREAM3D_REQUIRE_EQUAL(ok, true);
  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, false);

  isRelative = MXADir::isRelativePath(dirPath);
  DREAM3D_REQUIRE_EQUAL(isRelative, false);
  isAbsolute = MXADir::isAbsolutePath(dirPath);
  DREAM3D_REQUIRE_EQUAL(isAbsolute, true);


#if defined (WIN32)
    dirPath = "C:\\";
#else
  dirPath = "/tmp";
#endif

  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, true);

  std::string path = MXADir::cleanPath(dirPath);
  DREAM3D_REQUIRE_EQUAL(path.compare(dirPath), 0)

  err = MXADir::mkdir(dirPath, true);
  DREAM3D_REQUIRE_EQUAL(err, 1);

  isDir = MXADir::isDirectory(dirPath);
  DREAM3D_REQUIRE_EQUAL(isDir, true);

  isFile = MXADir::isFile(dirPath);
  DREAM3D_REQUIRE_EQUAL(isFile, false);

  exists = MXADir::exists(dirPath);
  DREAM3D_REQUIRE_EQUAL(exists, true);


  isRelative = MXADir::isRelativePath(dirPath);
  DREAM3D_REQUIRE_EQUAL(isRelative, false);

  isAbsolute = MXADir::isAbsolutePath(dirPath);
  DREAM3D_REQUIRE_EQUAL(isAbsolute, true);

#ifndef _MSC_VER
  path = MXADir::cleanPath("/tmp/");
  if (path.compare("/tmp") != 0)
  {
      std::cout << "This is bad" << std::endl;
  }

  path = MXADir::cleanPath("/tmp/Other");
  if (path.compare("/tmp/Other") != 0)
  {
      std::cout << "This is bad" << std::endl;
  }
#endif


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RemoveDirectoriesTest()
{
  std::cout  << "|- RemoveDirectoriesTest -----------------" << std::endl;
  int err = 0;
  bool ok;
  std::string testdir = MXAUnitTest::MXATempDir + MXAUnitTest::MXAFileSystemPathTest::TestDir;
  std::cout << "|-- Removing top level test dir: '" << testdir << "'" << std::endl;
  ok = MXADir::rmdir(testdir, false);
  DREAM3D_REQUIRE_EQUAL(ok, true);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DirListTest()
{
  std::cout  << "|- DirListTest -----------------" << std::endl;

  int err = 0;
  std::vector<std::string> list = MXADir::entryList(MXAUnitTest::MXABuildDir);

  for (std::vector<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter ) {
    std::cout << "|--- DIR_LIST_ENTRY--> " << *iter << std::endl;
  }

  std::string ppath = MXADir::parentPath(MXAUnitTest::MXATestBinaryDirectory);
  //std::cout << "Child Path: " << MXAUnitTest::MXATestBinaryDirectory << std::endl;
  //std::cout << "ppath: " << ppath << std::endl;
  //std::cout << "Ref Path:   " << MXAUnitTest::MXABuildDir << std::endl;
  int equal  = ppath.compare(MXAUnitTest::MXABuildDir);
  DREAM3D_REQUIRE_EQUAL(equal, 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileNameExtensionTest()
{
  std::cout  << "|- FileNameExtensionTest -----------------" << std::endl;
  int err = 0;
#if 0
  std::string file("SomeFile.txt");
  std::string base = MXADir::fileNameWithOutExtension(file);
  DREAM3D_REQUIRE_EQUAL(base, "SomeFile");

  file ="SomeFile";
  base = MXADir::fileNameWithOutExtension(file);
  DREAM3D_REQUIRE_EQUAL(base, "SomeFile");


  file ="SomeFile.";
  base = MXADir::fileNameWithOutExtension(file);
  DREAM3D_REQUIRE_EQUAL(base, "SomeFile.");


  file ="SomeFile.txt.bin";
  base = MXADir::fileNameWithOutExtension(file);
  DREAM3D_REQUIRE_EQUAL(base, "SomeFile.txt");

  file ="/SomePath/To/SomeFile.txt.bin";
  base = MXADir::fileNameWithOutExtension(file);
  DREAM3D_REQUIRE_EQUAL(base, "SomeFile.txt");
#endif


  std::string testdir = MXAUnitTest::MXATempDir + MXAUnitTest::MXAFileSystemPathTest::TestDir + MXAUnitTest::DirSeparator;

  std::string fnBase = ".hidden_file";
  std::string test = ".hidden_file";
  std::string testFileName = testdir + fnBase;
  std::string ext = "";
  std::string fnWoExt = MXAFileInfo::fileNameWithOutExtension(testFileName);
  DREAM3D_REQUIRE_EQUAL(fnWoExt, test);


  fnBase = "Normal.txt";
  test = "Normal";
  testFileName = testdir + fnBase;
  fnWoExt = MXAFileInfo::fileNameWithOutExtension(testFileName);
  DREAM3D_REQUIRE_EQUAL(fnWoExt, test);

  fnBase = "No_Extension";
  test = "No_Extension";
  testFileName = testdir + fnBase;
  fnWoExt = MXAFileInfo::fileNameWithOutExtension(testFileName);
  DREAM3D_REQUIRE_EQUAL(fnWoExt, test);

  fnBase = "EndsWithDot.";
  test = "EndsWithDot";
  testFileName = testdir + fnBase;
  fnWoExt = MXAFileInfo::fileNameWithOutExtension(testFileName);
  DREAM3D_REQUIRE_EQUAL(fnWoExt, test);


  fnBase = "Normal.txt.bin";
  test = "Normal.txt";
  testFileName = testdir + fnBase;
  fnWoExt = MXAFileInfo::fileNameWithOutExtension(testFileName);
  DREAM3D_REQUIRE_EQUAL(fnWoExt, test);
  ext = MXAFileInfo::extension(testFileName);
  DREAM3D_REQUIRE_EQUAL(ext, "bin");

  fnBase = ".txt.bin";
  test = ".txt";
  testFileName = testdir + fnBase;
  fnWoExt = MXAFileInfo::fileNameWithOutExtension(testFileName);
  DREAM3D_REQUIRE_EQUAL(fnWoExt, test);
  ext = MXAFileInfo::extension(testFileName);
  DREAM3D_REQUIRE_EQUAL(ext, "bin");

  fnBase = "";
  test = "";
  testFileName = fnBase;
  fnWoExt = MXAFileInfo::fileNameWithOutExtension(testFileName);
  DREAM3D_REQUIRE_EQUAL(fnWoExt, test);
  ext = MXAFileInfo::extension(testFileName);
  DREAM3D_REQUIRE_EQUAL(ext, "");


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestTempDir()
{
  std::string tmp = MXADir::tempPath();
  std::cout << "tmp = " << tmp << std::endl;
}

// -----------------------------------------------------------------------------
//  Use Boost unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestTempDir() );
  DREAM3D_REGISTER_TEST( MakeDirectoriesTest() )
  DREAM3D_REGISTER_TEST( FilesTest() )
  DREAM3D_REGISTER_TEST( FileNameTest() )
  DREAM3D_REGISTER_TEST( AbsolutePathTest() )
  DREAM3D_REGISTER_TEST( DirListTest() )
  DREAM3D_REGISTER_TEST( RemoveDirectoriesTest() )
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  DREAM3D_REGISTER_TEST( FileNameExtensionTest() )
  PRINT_TEST_SUMMARY();
  return err;
}


