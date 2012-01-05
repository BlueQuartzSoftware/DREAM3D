/* ============================================================================
 * Copyright (c) 2008 Jason Gruber jgruber@andrew.cmu.edu
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Jason Gruber, Carnegie-Mellon University nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was partly written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include "Smoothing.h"

#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <limits>

#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/StringUtils.h"

#include "DREAM3DLib/Common/Constants.h"
#include "LinearAlgebra.h"
#include "Triangle.hpp"
#include "Node.hpp"

#ifdef _WIN32
const std::string Separator("\\");
#else
const std::string Separator("/");
#endif


#define MAKE_UPDATES_FILE_PATH(outpath, filename)\
    std::string outpath;\
{ std::stringstream ss;\
ss.setf(std::ios::fixed);\
ss.fill('0');\
  ss << m_OutputDirectory << MXADir::Separator << m_OutputFilePrefix << filename << "_" << std::setw(width) << updates << ".inp";\
 outpath = ss.str();\
}

#define MAKE_OUTPUT_FILE_PATH(outpath, filename)\
    std::string outpath = m_OutputDirectory + MXADir::Separator + m_OutputFilePrefix + filename;





/**
 * @brief
 * @param str
 * @param tokens
 * @param delimiters
 */
void tokenize(const std::string& str,
        std::vector<std::string>& tokens,
        const std::string& delimiters = " ")
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
    {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));

    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);

    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
    }
}

/**
 * @brief
 * @param i
 * @param j
 * @return
 */
inline int delta(int i, int j) {return (i==j);}


using namespace smooth;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Smoothing::Smoothing()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Smoothing::~Smoothing()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Smoothing::execute()
{
 // int err = 0;

  // Data variables
  int nnod, ntri;
  std::vector<std::string> data;

  std::cout << "Interval for printout: " << m_OutputInterval << std::endl;

  if (m_LockQuads == true)
  {
    std::cout << "Will not allow quad points to move " << std::endl;
  }
  else
  {
    std::cout << "Quad points are free to move " << std::endl;
  }

  FILE* nodesFile = fopen(m_NodesFile.c_str(), "rb+");
  if (nodesFile == NULL)
  {
    std::cout << "Error opening nodes file '" << m_NodesFile << "'" << std::endl;
    setErrorCondition(-1);
    return;
  }
  FILE* triangleFile = fopen(m_TrianglesFile.c_str(), "rb+");
  if (triangleFile == NULL)
  {
    std::cout << "Error opening Triangles file '" << m_TrianglesFile << "'" << std::endl;
    setErrorCondition(-1);
    return;;
  }

  // read in nodes/triangles
  // Calculate how many nodes are in the file:
  fseek(nodesFile, 0, SEEK_END);
  size_t fLength = ftell(nodesFile);
  nnod = fLength/20;
  fseek(nodesFile, 0, SEEK_SET);
  fLength = ftell(nodesFile);
  if(0 != fLength)
  {
    assert(0 != fLength);
  }
  std::cout << "Number of nodes: " << nnod << std::endl;
  // Allocated the Nodes
  std::vector<node> nodes(nnod);
  float max[3] = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };
  float min[3] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };

  //Read the nodes
  std::cout << "reading nodes " << std::endl;
  size_t nread = 0;
  unsigned char nodeData[20];
  int* nodeId = (int*)(&nodeData[0]);
  int* nodeKind = (int*)(&nodeData[4]);
  float* vec3f = (float*)(&nodeData[8]);
  for (int i = 0; i < nnod; i++)
  {
    nread = fread(nodeData, 20, 1, nodesFile); // Read one set of Node Kind from the nodes file
    if (nread != 1)
    {
    	return;
    }
    node& node = nodes[*nodeId];
    node.setValues(*nodeId, *nodeKind, vec3f);
    node.nId = *nodeId;
    for(int i = 0; i < 3; ++i)
    {
      if (vec3f[i] > max[i]) { max[i] = vec3f[i]; }
      if (vec3f[i] < min[i]) { min[i] = vec3f[i]; }
    }
  }
//  std::cout << "Number Nodes: " << nnod << std::endl;
  std::cout << "Model Dimensions: " << std::endl;
  for (int i = 0; i < 3; i++) {
    std::cout << min[i] << " " << max[i] << std::endl;
  }
  std::cout << "end reading nodes" << std::endl;
  fclose(nodesFile);
  // Read the Triangles File
  fseek(triangleFile, 0, SEEK_END);
  fLength = ftell(triangleFile);
  ntri = fLength/24;
  fseek(triangleFile, 0, SEEK_SET);
  fLength = ftell(triangleFile);
  if(0 != fLength)
  {
    assert(0 != fLength);
  }
  std::cout << "Number of triangles: " << ntri << std::endl;

  //Allocate the triangle and node vectors
  std::vector<triangle> triangles(ntri);
  std::vector<int> tid(ntri);
  std::vector<int> nodetype(nnod);


  //Read the triangles
  std::cout << "reading triangles: " << std::endl;
  int tData[6];
  for (int i = 0; i < ntri; i++)
  {

    nread = fread(tData, sizeof(int), 6, triangleFile);
    tid[i] = tData[0];
    triangles[i].region1 = tData[4];
    triangles[i].region2 = tData[5];
//    triangleFile >> tid[i] >> nid[0] >> nid[1] >> nid[2] >> junk >> junk >> junk >> triangles[i].region1 >> triangles[i].region2;

//    triangles[i](0) = &nodes[tData[1]];
//    triangles[i](1) = &nodes[tData[2]];
//    triangles[i](2) = &nodes[tData[3]];
    triangles[i].setNodes(&nodes[tData[1]], &nodes[tData[2]], &nodes[tData[3]]);
  }
  std::cout << "end reading triangles" << std::endl;
  fclose(triangleFile);

  //Allocate vectors and matricies
  int n_size = 3 * nodes.size();
  Vector<float> x(n_size), F(n_size);
  SMatrix<float> K(n_size, n_size);

  //Allocate constants for solving linear equations
  const float epsilon = 1.0; // change this if quality force too
  // high, low
  const float dt = (40.0e-6) * (10 / max[1]); // time step, change if
  // mesh moves too much,
  // little
  float bc_dt; // for updating
  const float small = 1.0e-12f;
  const float large = std::numeric_limits<float>::max();
  const float one12th = 1.0f / 12.0f;
  const float tolerance = 1.0e-5f; // Tolerance for nodes that are
  // near the RVE boundary

  float A_scale, Q_scale; // Pre-factors for quality and curvature forces
  //   don't make these two values too far different
  //  larger values should increase velocities

  //Variables for logging of quality progress
  float Q_max, Q_sum, Q_ave, Q_max_ave;
  int hist_count = 10;

  std::vector<float> Q_max_hist(hist_count);

  Q_ave = 2.9f;
  Q_max_ave = 10.0f;

  A_scale = 4000.0f;
  Q_scale = 500.0f;

  // update loop
  size_t count = 1;
  int width = 0;
  while (count < m_Iterations)
  {
    ++width;
    count *= 10;
  }

  for (size_t updates = 1; updates <= m_Iterations; ++updates)
  {
    // changed arg index
    std::cout << "Update loop: " << updates << std::endl;

    // compute triangle contributions to K and F
    int ntri = triangles.size();
    Q_max = 0;
    Q_sum = 0;
    for (int t = 0; t < ntri; t++)
    { // Loop through number of triangles
      triangle& rtri = triangles[t];
      Vector<float> n(3);
      n = rtri.normal();
      float A = rtri.area();
      float Q = rtri.circularity(A);
      Q_sum += Q;
      if (Q > Q_max) Q_max = Q;
      for (int n0 = 0; n0 < 3; n0++)
      {
        int i = rtri.getNode(n0).nId;
        for (int j = 0; j < 3; j++)
        {
          node& aNode = nodes[i];
          aNode.getPos(j) += small;
//          nodes[i][j] += small;
          float Anew = rtri.area();
          float Qnew = rtri.circularity(Anew);
          aNode.getPos(j) -= small;

          F[3 * i + j] -= (A_scale * (Anew - A) + Q_scale * (Qnew - Q) * A) / small;
        }
        for (int n1 = 0; n1 < 3; n1++)
        {
          int h = rtri.getNode(n1).nId;
          for (int k = 0; k < 3; k++) {
            for (int j = 0; j < 3; j++) {
              K[3 * h + k][3 * i + j] += one12th * (1.0f + delta(i, h)) * n[j] * n[k] * A;}}
        }
      }
    }

    // add epsilon to the diagonal
    for (size_t r = 0; r < nodes.size(); r++) {
      for (int s = 0; s < 3; s++) {
        K[3 * r + s][3 * r + s] += epsilon; }}

    // apply boundary conditions
    // if node i, component j is constrained, do this...
    // K[3*i+j][3*i+j] += large;
    std::cout << "Applying Boundary Conditions...." << std::endl;
    for (size_t r = 0; r < nodes.size(); r++)
    {
      node& aNode = nodes[r];
      for (int s = 0; s < 3; s++)
      {
        if ( fabs(aNode.getPos(s) - max[s]) < tolerance) { K[3 * r + s][3 * r + s] += large; }
        if ( fabs(aNode.getPos(s) - min[s]) < tolerance) { K[3 * r + s][3 * r + s] += large; }
      }
    }
    std::cout << "Solving for Node Velocities...." << std::endl;
    // solve for node velocities
    int iterations = CR(K, x, F, 4000, 1.0e-5f);
    std::cout << iterations << " iterations ... " << std::endl;

    //Update the quality information
    if (updates - 1 < Q_max_hist.size())
    {
      Q_max_hist[updates - 1] = Q_max;
    }
    else
    {

      //Update the history of Q_max
      for (size_t i = 0; i < Q_max_hist.size() - 1; i++)
      {
        // std::cout << i << " "<< Q_max_hist[i] << " " << Q_max_hist[i+1] << std::endl;
        Q_max_hist[i] = Q_max_hist[i + 1];
      }

      Q_max_hist[Q_max_hist.size() - 1] = Q_max;

      //Compute the rolling average of the Q_max
      Q_max_ave = 0;
      for (size_t i = 0; i < Q_max_hist.size(); i++)
        Q_max_ave += Q_max_hist[i];
      Q_max_ave /= Q_max_hist.size();
    }
    Q_ave = Q_sum / ntri;

    //  for(int i=0; i<Q_max_hist.size(); i++)
    //    std::cout<<"Q_Max history ... "<<Q_max_hist[i]<<std::endl;

    std::cout << "Maximum quality ... " << Q_max << std::endl;
    std::cout << "Ave_Max quality ... " << Q_max_ave << std::endl;
    std::cout << "Average quality ... " << Q_ave << std::endl;
    std::cout << std::endl;

    MAKE_UPDATES_FILE_PATH(IterationFile, DREAM3D::SurfaceSmoothing::Smooth3DIterationFile)



    // update node positions
    for (size_t r = 0; r < nodes.size(); r++)
    {
      node& aNode = nodes[r];
      for (int s = 0; s < 3; s++)
      {
        bc_dt = dt;
        if ((fabs(aNode.getPos(s) - max[s]) < tolerance) || (fabs(aNode.getPos(s) - min[s]) < tolerance)) bc_dt = 0.0;

        if (fabs(dt * x[3 * r + s]) > 1.0) { aNode.getPos(s) += 0.0; }
        else if (fabs(dt * x[3 * r + s]) < 1.0) { aNode.getPos(s) += bc_dt * x[3 * r + s]; }
      }
    }

    if (!((updates) % m_OutputInterval))
    {
      // Open the outputfile
      std::ofstream inpFileOStream;
      inpFileOStream.open(IterationFile.c_str(), std::ios_base::binary);
      if (!inpFileOStream)
      {
        std::cout << "Failed to open: " << IterationFile << std::endl;
        setErrorCondition(-1);
        return;
      }

      inpFileOStream << nnod << " " << ntri << " 0 2 0" << std::endl;
      // the "0 2 0" signals the presence of upID downID output
      for (int ia = 0; ia < nnod; ia++) {
        node& n = nodes[ia];
        inpFileOStream << n.nId << " " << n.getPos(0) << " " << n.getPos(1) << " " << n.getPos(2) << std::endl;
      }

      for (int i = 0; i < ntri; i++)
      {
        //    inpfile<<i<<" "<<triangles[i].region1>>triangles[i].region2<<" tri ";
        inpFileOStream << i << " 0 tri ";
        node& n0 = triangles[i].getNode(0);
        node& n1 = triangles[i].getNode(1);
        node& n2 = triangles[i].getNode(2);
        inpFileOStream << n0.nId << " ";
        inpFileOStream << n1.nId << " ";
        inpFileOStream << n2.nId << std::endl;
        //inpfile<<triangles[i].region1<<" ";
        //inpfile<<triangles[i].region2<<std::endl;
      }

      //    for(int i=0; i<data.size(); i++)

      inpFileOStream << "2 1 1" << std::endl;
      inpFileOStream << "minID, none" << std::endl;
      inpFileOStream << "maxID, none" << std::endl;
      for (int i = 0; i < ntri; i++) // ADR  25 jun 09
        inpFileOStream << i << " " << triangles[i].region1 << " " << triangles[i].region2 << std::endl;

      //    for (int i=0; i<ntri; i++) {
      //    int r1 = triangles[i].region1;
      //    int r2 = triangles[i].region2;
      //    int min = (r1<r2 ? r1:r2);
      //    int max = (r1>r2 ? r1:r2);
      //    inpfile<<i<<" "<<min<<" "<<max<std::endl;
      //    }

      inpFileOStream.close();

      MAKE_OUTPUT_FILE_PATH(nodes_smoothed_txt, DREAM3D::SurfaceSmoothing::NodesSmoothedFile);

      inpFileOStream.open(nodes_smoothed_txt.c_str(), std::ios_base::binary); // output Sukbin style nodes
      if (!inpFileOStream)
      {
        std::cout << "Failed to open: " << nodes_smoothed_txt << std::endl;
        setErrorCondition(-1);
        return;
      }
      inpFileOStream << nnod << std::endl;
      for (int ia = 0; ia < nnod; ia++) {
        node& n0 = nodes[ia];
        inpFileOStream << n0.nId << " " << nodetype[ia] << " " << n0.getPos(0) << " " << n0.getPos(1)<< " " << n0.getPos(2) << std::endl;
      }
      inpFileOStream.close();
    }

  }

  std::cout << "Writing new Nodes File...." << std::endl;
  // Over Write the Nodes file with the new positions.
  static const size_t BYTE_COUNT = 20;
  size_t totalWritten = 0;
  FILE* f = NULL;

  // Create a new file, over writing our current file
  f = fopen(m_NodesFile.c_str(), "wb");
  if (NULL == f)
  {
    setErrorCondition(-1);
    return;
  }

  // Write each node to the file
  for (int k = 0; k < nnod; k++)
  {
    *nodeId = nodes[k].nId;
    *nodeKind = nodes[k].nType;
    vec3f[0] = nodes[k].getPos(0);
    vec3f[1] = nodes[k].getPos(1);
    vec3f[2] = nodes[k].getPos(2);
    //      fprintf(f, "%d %d %f %f %f\n", tID, nk, x, y, z);
    totalWritten = fwrite(nodeData, sizeof(unsigned char), BYTE_COUNT, f);
    if (totalWritten != BYTE_COUNT)
    {
      std::cout << "Not enough data written to the Nodes file." << std::endl;
      fclose(f); // Close the Nodes file.
      setErrorCondition(-1);
      return;
    }

  }
  fclose(f); // Close the Nodes file.

  setErrorCondition(0);
}
