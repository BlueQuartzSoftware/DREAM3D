///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

//using namespace std;



#include <MXA/Common/LogTime.h>
#include <MXA/Utilities/MXALogger.h>
#include <MXA/Utilities/MXAFileSystemPath.h>

#include <AIM/Common/Constants.h>
#include <AIM/Common/MicroGen3D.h>
#include <AIM/Common/AIMVersion.h>

#include <string>
#include <iostream>

//-- Boost Program Options
#include <boost/program_options.hpp>



#define CHECK_ARG(var, mandatory)\
    if (vm.count(#var) > 1) { mxa_log << logTime() << "Multiple Occurances for Parameter " << #var << std::endl; }\
    if (vm.count(#var) == 0 && mandatory == true) { \
    mxa_log << "Parameter --" << #var << " ==> Required. Program will Terminate." << std::endl; }\
    if (vm.count(#var) == 0 && mandatory == false) { \
    mxa_log << "--" << #var << " Using Default: '"  << var << "'" << std::endl; }
#define CHECK_BOOL_ARG(var)\
  mxa_log << "--" << #var << " is ";\
  if (var == true) { mxa_log << "TRUE"; } else { mxa_log << "FALSE"; }\
  mxa_log << "" << std::endl;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::cout << logTime() << "Starting Reconstruction ... " << std::endl;
  MXALOGGER_METHOD_VARIABLE_INSTANCE

  std::string inputFile;
  double sizex = 0.0;
  double sizey = 0.0;
  double sizez = 0.0;

  double resx = 0.0;
  double resy = 0.0;
  double resz = 0.0;

  bool mergetwinsoption = false;
  int32 minallowedgrainsize = 0.0;
  double minseedconfidence = 0.0;
  double misorientationtolerance = 0.0;

  int32 crystruct = 1;
  bool alreadyformed = false;

  std::string logFile;

  // Handle program options passed on command line.
  boost::program_options::options_description desc("Possible Parameters");
  desc.add_options()
  ("help", "Produce help message")
  ("inputFile,i", boost::program_options::value<std::string>(&inputFile), "REQUIRED: Input File")
  ("xdim,x", boost::program_options::value<double>(&sizex), "x dimension of your volume")
  ("ydim,y", boost::program_options::value<double>(&sizey), "y dimension of your volume")
  ("zdim,z", boost::program_options::value<double>(&sizez), "z dimension of your volume")
  ("xres", boost::program_options::value<double>(&resx), "x resolution of your volume")
  ("yres", boost::program_options::value<double>(&resy), "y resolution of your volume")
  ("zres", boost::program_options::value<double>(&resz), "z resolution of your volume")
  ("merge-twins", boost::program_options::bool_switch(&mergetwinsoption), "Do you want to merge twins")
  ("minallowedgrainsize", boost::program_options::value<int32>(&minallowedgrainsize), "What is the minimum allowed grain size")
  ("minseedconfidence", boost::program_options::value<double>(&minseedconfidence), "What is the minimum allowed confidence")
  ("misorientationtolerance", boost::program_options::value<double>(&misorientationtolerance), "What is the misorientation tolerance (degrees)")
  ("crystalstructure", boost::program_options::value<int32>(&crystruct), "Do you have a HCP (1) or FCC (2) material")
  ("alreadyformed", boost::program_options::bool_switch(&alreadyformed), "Have you already formed grains (1) Yes (0) No")
  ("logfile,l", boost::program_options::value<std::string>(&logFile), "Name of the Log file to store any output into")
  ;

try
{


  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  // Print help message if requested by user and return.
  if (vm.count("help") || argc < 2)
  {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }
  if (vm.count("logfile") != 0)
  {
    logFile = MXAFileSystemPath::toNativeSeparators(logFile);
  }
  if (false == logFile.empty())
  {
    mxa_log.open(logFile);
  }
  mxa_log << logTime() << "Reconstruction Version " << AIMRepresentation::Version::Complete << " Starting " << std::endl;

  mxa_log << "Parameters being used are: " << std::endl;
  CHECK_ARG( inputFile, true);
  CHECK_ARG( sizex, true);
  CHECK_ARG( sizey, true);
  CHECK_ARG( sizez, true);
  CHECK_ARG( resx, true);
  CHECK_ARG( resy, true);
  CHECK_ARG( resz, true);
  CHECK_BOOL_ARG( mergetwinsoption);
  CHECK_ARG( minallowedgrainsize, true);

  CHECK_ARG( minseedconfidence, true);
  CHECK_ARG( misorientationtolerance, true);
  CHECK_ARG( crystruct, true);
  CHECK_BOOL_ARG( alreadyformed);

#if 0
  MicroGen3D* m = NULL;
  m = new MicroGen3D;
  m->initialize(sizex, sizey, sizez, resx, resy, resz, mergetwinsoption, minallowedgrainsize, minseedconfidence,
                misorientationtolerance, crystruct, alreadyformed);
#endif

  MicroGen3D microgen;
#if 0
  microgen.initialize(sizex, sizey, sizez, resx, resy, resz, mergetwinsoption, minallowedgrainsize, minseedconfidence,
                      misorientationtolerance, crystruct, alreadyformed);


  int32 numgrains = 0;
  if(alreadyformed == false)
  {
    microgen.loadSlices();
    numgrains = microgen.form_grains();
    microgen.remove_smallgrains();
    numgrains = microgen.renumber_grains1();
    microgen.write_volume1(AIM::Reconstruction::ReconstructedDataFile);
  }
  if(alreadyformed == true)
  {
    microgen.load_data(AIM::Reconstruction::InputFile);
  }
  for(int iter1=0;iter1<180;iter1++)
  {
    for(int iter2=0;iter2<180;iter2++)
    {
      for(int iter3=0;iter3<180;iter3++)
      {
        microgen.eulerrank[iter1][iter2][iter3] = 0;
      }
    }
  }
  microgen.homogenize_grains();
  microgen.assign_badpoints();
  microgen.find_neighbors();
  microgen.merge_containedgrains();
  numgrains = microgen.renumber_grains2();
  microgen.write_volume1(AIM::Reconstruction::ReconstructedDataFile);
  if(mergetwinsoption == 1)
  {
    microgen.merge_twins();
    microgen.characterize_twins();
  }
  numgrains = microgen.renumber_grains3();
  microgen.write_volume2(AIM::Reconstruction::ReconstructedDataFile);
  microgen.find_goodneighbors();
  microgen.find_centroids();
  microgen.find_moments();
  microgen.find_axes();
  microgen.find_vectors();
  microgen.measure_misorientations();
  microgen.find_colors();
  microgen.find_convexities();
  microgen.volume_stats(AIM::Reconstruction::StatsFile, AIM::Reconstruction::VolBinFile, AIM::Reconstruction::BOverABinsFile,
                        AIM::Reconstruction::COverABinsFile, AIM::Reconstruction::COverBBinsFile, AIM::Reconstruction::SVNFile, AIM::Reconstruction::SVSFile,
                        AIM::Reconstruction::MisorientationBinsFile, AIM::Reconstruction::MicroBinsFile);
  microgen.write_volume2(AIM::Reconstruction::ReconstructedDataFile);
  microgen.create_visualization(AIM::Reconstruction::ReconstructedVisualizationFile);
  microgen.write_grains(AIM::Reconstruction::GrainsFile);
  microgen.write_axisorientations(AIM::Reconstruction::AxisOrientationsFile);
  microgen.write_eulerangles(AIM::Reconstruction::EulerAnglesFile);
  microgen.find_boundarycenters(AIM::Reconstruction::BoundaryCentersFile);
  #endif
}
catch (...)
{
 std::cout << "Error on Input: Displaying help listing instead. **" << std::endl;
 std::cout << desc << std::endl;
 for (int i = 0; i < argc; ++i)
 {
   std::cout << argv[i] << std::endl;
 }
 return EXIT_FAILURE;
}

  return EXIT_SUCCESS;
}

