/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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


#ifndef _importasciidatawizard_h_
#define _importasciidatawizard_h_

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
    ImportASCIIDataWizard(const QString &inputFilePath, int numLines, QWidget* parent = NULL);

    virtual ~ImportASCIIDataWizard();

    static const int TotalPreviewLines = 50;

    /**
    * @brief Static function that converts the boolean delimiter values to a list of delimiter characters
    */
    static QList<char> ConvertToDelimiters(bool tabAsDelimiter, bool semicolonAsDelimiter, bool commaAsDelimiter, bool spaceAsDelimiter);

    /**
    * @brief Static function that will read 'numOfLines' lines from the file at 'inputFilePath', starting at line 'beginIndex'
    * @param inputFilePath The path to the file
    * @param beginLine The line to begin reading at in the file
    * @param numOfLines The number of lines to read from the file
    */
    static QStringList ReadLines(const QString &inputFilePath, int beginLine, int numOfLines);

    /**
    * @brief Static convenience function that will read a certain line from the file 'inputFilePath'.
    * @param inputFilePath The path to the file
    * @param line The line to read from the file
    */
    static QString ReadLine(const QString &inputFilePath, int line);

    /**
    * @brief Static function that loads lines into the table objects.  The full lines are stored in the ASCIIDataItem object that backs up each table item, and are not actually displayed in the table yet.
    * @param inputFilePath The path to the file to load
    * @param beginLine The line to begin loading at in the file
    */
    static void LoadOriginalLines(QStringList lines);

    /**
    * @brief Static function that will split the lines argument into a list of tokens based on delimiter and fixed width settings from the wizard
    * @param lines The lines to tokenize
    * @param delimiters List of delimiters
    * @param isFixedWidth Boolean that identifies if the fixed width radio button is checked or not.
    * @param consecutiveDelimiters Boolean that identifies if the consecutive delimiters checkbox is checked or not.
    */
    static QList<QStringList> TokenizeLines(QStringList lines, QList<char> delimiters, bool isFixedWidth, bool consecutiveDelimiters);

    /**
    * @brief Static function that will split the line argument into a list of tokens based on delimiter and fixed width settings from the wizard
    * @param line The line to tokenize
    * @param delimiters List of delimiters
    * @param isFixedWidth Boolean that identifies if the fixed width radio button is checked or not.
    * @param consecutiveDelimiters Boolean that identifies if the consecutive delimiters checkbox is checked or not.
    */
    static QStringList TokenizeLine(QString line, QList<char> delimiters, bool isFixedWidth, bool consecutiveDelimiters);

    /**
    * @brief Static function that will insert the tokenized lines into the preview table.
    * @param tokenizedLines List of tokenized lines
    * @param firstRowHeaderIndex The name of the first row's header (an index, such as '2' or '5')
    */
    static void InsertTokenizedLines(QList<QStringList> tokenizedLines, int firstRowHeaderIndex);

    /**
    * @brief Static function that inserts the full strings stored in each row item into one column in the preview table.
    * @param lines List of lines
    * @param firstRowHeaderIndex The name of the first row's header (an index, such as '2' or '5')
    */
    static void InsertLines(QStringList lines, int firstRowHeaderIndex);

    QList<char> getDelimiters();
    bool getConsecutiveDelimiters();
    bool getHasFixedWidth();
    QStringList getHeaders();
    QStringList getDataTypes();
    int getBeginningLineNum();
    QString getInputFilePath();
    QVector<size_t> getTupleDims();

    void setInputFilePath(const QString &inputFilePath);

  protected slots:
    void refreshModel();

  private:
    QString                                             m_InputFilePath;
    int                                                 m_NumLines;

    QPushButton*                                        m_RefreshBtn;

    ImportASCIIDataWizard(const ImportASCIIDataWizard&); // Copy Constructor Not Implemented
    void operator=(const ImportASCIIDataWizard&); // Operator '=' Not Implemented
};

#endif /* ImportASCIIDataWizard_H_ */
