


#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <iostream>
#include <QtCore/QString>

#define BUF_SIZE 512

int main(int argc, char** argv)
{

  if (argc != 3)
  {
    std::cout << "This program needs 2 arguments: Input Ph file and output Ph file." << std::endl;
    return EXIT_FAILURE;
  }
  std::string inputFile(argv[1]);
  std::string outputFile(argv[2]);

  FILE* f = fopen(inputFile.toLatin1().data(), "r");
  if(f == NULL)
  {
    std::cout << "Error opening input file" << std::endl;
    return EXIT_FAILURE;
  }

  FILE* out = fopen(outputFile.toLatin1().data(), "wb");
  if (NULL == out)
  {
    std::cout << "Error opening output file" << std::endl;
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
  std::cout << "Fixing Completed" << std::endl;
  return EXIT_SUCCESS;

}
