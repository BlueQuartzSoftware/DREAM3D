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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FileReader.h"

using namespace DREAM3D;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileReader::FileReader()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileReader::~FileReader()
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileReader::parseFloat3V(const char* input, float* output, float defaultValue)
{
  char text[256];
  int n = sscanf(input, "%s %f %f %f", text, &(output[0]), &(output[1]), &(output[2]) );
  if (n != 4)
  {
    output[0] = output[1] = output[2] = defaultValue;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileReader::parseInt3V(const char* input, int* output, int defaultValue)
{
  char text[256];
  int n = sscanf(input, "%s %d %d %d", text, &(output[0]), &(output[1]), &(output[2]) );
  if (n != 4)
  {
    output[0] = output[1] = output[2] = defaultValue;
    return -1;
  }
  return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileReader::nonPrintables(char* buf, size_t bufSize)
{
  int n = 0;
  for (size_t i = 0; i < bufSize; ++i)
  {
    if (buf[i] < 33 && buf[i] > 0) { n++; }
  }
  return n;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileReader::readLine(std::istream &in, char* buf, int bufSize)
{

  bool readAnotherLine = true;
  size_t gcount = in.gcount();
  while ( readAnotherLine == true && in.gcount() != 0) {
    // Zero out the buffer
    ::memset(buf, 0, bufSize);
    // Read a line up to a '\n' which will catch windows and unix line endings but
    // will leave a trailing '\r' at the end of the string
    in.getline(buf, bufSize, '\n');
    gcount = in.gcount();
    if (gcount > 1 && buf[in.gcount()-2] == '\r')
    {
      buf[in.gcount()-2] = 0;
    }
    int len = strlen(buf);
    int np = nonPrintables(buf, bufSize);
    if (len != np)
    {
      readAnotherLine = false;
    }

  }
  return static_cast<int>(in.gcount());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileReader::tokenize(const std::string& str,
                          std::vector<std::string>& tokens,
                          const std::string& delimiters)
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first "non-delimiter".
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

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
