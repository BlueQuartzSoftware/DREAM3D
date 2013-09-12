/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QtDebug>

#define BUF_SIZE 512

int main(int argc, char** argv)
{

  if (argc != 3)
  {
    qDebug() << "This program needs 2 arguments: Input Ph file and output Ph file." << "\n";
    return EXIT_FAILURE;
  }
  QString inputFile(argv[1]);
  QString outputFile(argv[2]);

  FILE* f = fopen(inputFile.toLatin1().data(), "r");
  if(f == NULL)
  {
    qDebug() << "Error opening input file" << "\n";
    return EXIT_FAILURE;
  }

  FILE* out = fopen(outputFile.toLatin1().data(), "wb");
  if (NULL == out)
  {
    qDebug() << "Error opening output file" << "\n";
    return EXIT_FAILURE;
  }

  char buf[BUF_SIZE];
  // Read Line #1 which has the dimensions
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, f);
  fprintf(out, "%s", buf);
  // Read Line #2 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, f);
  fprintf(out, "%s", buf);
  // Read Line #3 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, f);
  fprintf(out, "%s", buf);

  ::memset(buf, 0, BUF_SIZE);
  while ( fgets(buf, BUF_SIZE, f) )
  {
    // We have a buffer. lets make all unprintables into spaces
    int i = 0;
    int spaceCount = 0;
    while( i < BUF_SIZE) // While the character is NOT a new line charater
    {
      if (buf[i] > 47 && buf[i] < 58)
      {
        fprintf(out, "%c", buf[i]);
        if (buf[i+1] == '\n' || buf[i+1] == '\r')
        {

        }
        else if (spaceCount > 0 && buf[i+1] < 48 && buf[i+1] > 57)
        {
          fprintf(out, " ");
          spaceCount = 0;
        }
        i++;
      }

      if (buf[i] == 32)
      {
        spaceCount++;
      }

      if (buf[i] == '\n' || buf[i] == '\r')
      {
        fprintf(out, "\n");
        i++;
        break;
      }
      i++;
    }
    // Clear out the buffer
    ::memset(buf, 0, BUF_SIZE);
  }


  fclose(f);
  fclose(out);
  qDebug() << "Fixing Completed" << "\n";
  return EXIT_SUCCESS;

}
