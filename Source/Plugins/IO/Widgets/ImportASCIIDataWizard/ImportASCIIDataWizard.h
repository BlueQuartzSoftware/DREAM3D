/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _ImportASCIIDataWizard_H_
#define _ImportASCIIDataWizard_H_

#include <QtWidgets/QWizard>

struct ImportASCIIDataBundle
{
  QVector<QString>                            m_ColumnTypes;
  QVector<QString>                            m_ColumnNames;
  int                                         m_StartLine;
  char                                        m_Delimiter;
  int                                         m_DataSize;
};

Q_DECLARE_METATYPE(ImportASCIIDataBundle)

class ASCIIDataModel;

class ImportASCIIDataWizard : public QWizard
{
  Q_OBJECT

  public:
    enum WizardPages
    {
      DelimitedOrFixedWidth,
      Delimited,
      DataFormat
    };

    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    ImportASCIIDataWizard(const QString &inputFilePath, QWidget* parent = NULL);

    virtual ~ImportASCIIDataWizard();

    static const int TotalPreviewLines = 50;

    /**
    * @brief Static function that will read 'numOfLines' lines from the file at 'inputFilePath', starting at line 'beginIndex'
    * @param inputFilePath The path to the file to preview
    * @param beginLine The line to begin reading at in the file
    * @param numOfLines The number of lines to read from the file
    */
    static QVector<QString> ReadLines(const QString &inputFilePath, int beginLine, int numOfLines);

    /**
    * @brief Static function that loads lines from a file into the table.
      The full lines are stored in the ASCIIDataItem object that backs up each table item, and are not actually displayed in the table yet.
    * @param inputFilePath The path to the file to load
    * @param beginLine The line to begin loading at in the file
    */
    static void LoadLines(const QString &inputFilePath, int beginLine);

    /**
    * @brief Static function that will split each row's full string (stored in the item) into columns based on delimiter and fixed width settings from the wizard,
    and will insert the tokenized strings into the preview table.
    * @param isFixedWidth Boolean that identifies if the fixed width radio button is checked or not.
    * @param tabAsDelimiter Boolean that identifies if the tab checkbox is checked or not.
    * @param semicolonAsDelimiter Boolean that identifies if the semicolon checkbox is checked or not.
    * @param commaAsDelimiter Boolean that identifies if the comma checkbox is checked or not.
    * @param spaceAsDelimiter Boolean that identifies if the space checkbox is checked or not.
    * @param consecutiveDelimiters Boolean that identifies if the consecutive delimiters checkbox is checked or not.
    * @param firstRowHeaderIndex The name of the first row's header (an index, such as '2' or '5')
    */
    static void TokenizeAndInsertLines(bool isFixedWidth, bool tabAsDelimiter, bool semicolonAsDelimiter, bool commaAsDelimiter, bool spaceAsDelimiter, bool consecutiveDelimiters, int firstRowHeaderIndex);

    /**
    * @brief Static function that inserts the full strings stored in each row item into one column in the preview table.
    */
    static void InsertLines();

    void setInputFilePath(const QString &inputFilePath);

  private:
    QString                                             m_InputFilePath;

    ImportASCIIDataWizard(const ImportASCIIDataWizard&); // Copy Constructor Not Implemented
    void operator=(const ImportASCIIDataWizard&); // Operator '=' Not Implemented
};

#endif /* ImportASCIIDataWizard_H_ */
