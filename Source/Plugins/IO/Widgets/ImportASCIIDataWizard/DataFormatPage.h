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


#ifndef _dataformatpage_h_
#define _dataformatpage_h_

#include <QtWidgets/QWizardPage>

#include "AbstractWizardPage.h"

#include "ui_DataFormatPage.h"

class ASCIIDataModel;
class EditHeadersDialog;

class DataFormatPage : public AbstractWizardPage, private Ui::DataFormatPage
{
  Q_OBJECT

  public:
    /**
    * @brief Constructor
    * @param parameter The FilterParameter object that this widget represents
    * @param filter The instance of the filter that this parameter is a part of
    * @param parent The parent QWidget for this Widget
    */
    DataFormatPage(const QString &inputFilePath, int numLines, QWidget* parent = NULL);

    virtual ~DataFormatPage();

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();

    /**
    * @brief Refreshes the model
    */
    virtual void refreshModel();

    /**
    * @brief Launches the "Edit Headers" dialog
    */
    void launchEditHeadersDialog();

    /**
    * @brief Controls which page to navigate to after the user clicks "Next" button
    */
    virtual int nextId() const;

    /**
    * @brief Resets the page when the user hits the "Back" button
    */
    virtual void cleanupPage();

    TupleTableWidget* getTupleTable();

  protected:
    void showEvent(QShowEvent* event);

  protected slots:
    void on_startRowSpin_valueChanged(int i);
    void on_hasHeadersRadio_toggled(bool checked);
    void on_headersIndexLineEdit_textChanged(const QString &text);
    void on_editHeadersBtn_clicked();
    void on_dataTypeRadio_clicked();
    void on_skipRadio_clicked();
    void on_dataTypeCB_currentTextChanged(const QString &text);

    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void checkTupleDimensions(QVector<size_t> tupleDims);

  private:
    int                                             m_NumLines;
    EditHeadersDialog*                              m_EditHeadersDialog;

    void validateHeaders(QVector<QString> headers);

    DataFormatPage(const DataFormatPage&); // Copy Constructor Not Implemented
    void operator=(const DataFormatPage&); // Operator '=' Not Implemented
};

#endif /* DataFormatPage_H_ */
