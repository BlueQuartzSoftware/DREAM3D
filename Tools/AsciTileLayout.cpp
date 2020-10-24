#include <iostream>
#include <sstream>
#include <string>

#include <QtCore/QString>
#include <QtCore/QTextStream>

void createNewRow(int numCols, QTextStream& ss, int fieldWidth)
{

  QString s;
  QTextStream out(&s);
  for(int c = 0; c < numCols; c++)
  {
    out << ".";
    out.setFieldWidth(fieldWidth);
    out.setFieldAlignment(QTextStream::AlignRight);
    out.setPadChar('-');
    out << "-";
    out << "-";
  }
  out << "-.\n";
  ss << s;
}

void construct(int numRows, int numCols, int startCol, int endCol, int startRow, int endRow)
{
  QString out_s;
  QTextStream out(&out_s);
  int fieldWidth = 2;

  for(int r = 0; r < numRows; r++)
  {
    // ss << r << ":";
    createNewRow(numCols, out, fieldWidth); // Start the Row

    for(int c = 0; c < numCols; c++)
    {
      QString s;
      QTextStream ss(&s);

      ss << "|";
      ss.setFieldWidth(fieldWidth);
      ss.setFieldAlignment(QTextStream::AlignRight);
      ss.setPadChar(' ');
      if(r >= startRow && r <= endRow && c >= startCol && c <= endCol)
      {
        ss << r;
      }
      else
      {
        ss << "X";
      }
      ss.setFieldWidth(0);
      ss << ",";
      ss.setFieldWidth(fieldWidth);
      ss.setFieldAlignment(QTextStream::AlignRight);
      ss.setPadChar(' ');
      if(r >= startRow && r <= endRow && c >= startCol && c <= endCol)
      {
        ss << c;
      }
      else
      {
        ss << "X";
      }
      if(c == numCols - 1)
      {
        ss << "|\n"; // End the row
      }
      out << s;
    }
  }

  createNewRow(numCols, out, fieldWidth); // Start the Row

  std::cout << out_s.toStdString() << std::endl;
}

int main(int argc, char* argv[])
{
  construct(5, 8, 1, 5, 2, 8);
  return 0;
}
