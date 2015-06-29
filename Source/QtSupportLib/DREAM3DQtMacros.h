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


#ifndef _AIM_QTMACROS_H_
#define _AIM_QTMACROS_H_


#define READ_CHECKBOX_SETTING(prefs, prefix, tag, emptyValue)\
  { QString s = prefs.value(#tag).toString();\
    if (s.isEmpty() == false) {\
      bool bb = prefs.value(#tag).toBool();\
      prefix##tag->setChecked(bb); } else { prefix##tag->setChecked(emptyValue); } }



#define READ_STRING_SETTING(prefs, prefix, var, emptyValue)\
  prefix##var->setText( prefs.value(#var).toString() );\
  if (prefix##var->text().isEmpty() == true) { prefix##var->setText(emptyValue); }

#define READ_FILEPATH_SETTING(prefs, prefix, var, emptyValue)\
  prefix##var->blockSignals(true);\
  QString var = prefs.value(#var).toString();\
  var = QDir::toNativeSeparators(var);\
  prefix##var->setText(var );\
  verifyPathExists(var, prefix##var);\
  if (prefix##var->text().isEmpty() == true) { prefix##var->setText(emptyValue); }\
  prefix##var->blockSignals(false);

#define READ_SETTING(prefs, prefix, var, ok, temp, default, type)\
  ok = false;\
  temp = prefs.value(#var).to##type(&ok);\
  if (false == ok) {temp = default;}\
  prefix##var->setValue(temp);

#define READ_BOOL_SETTING(prefs, prefix, tag, emptyValue)\
  { QString s = prefs.value(#tag).toString();\
    if (s.isEmpty() == false) {\
      bool bb = prefs.value(#tag).toBool();\
      prefix##tag = (bb); } else { prefix##tag = (emptyValue); } }

#define WRITE_BOOL_SETTING(prefs, prefix, tag,  b)\
  prefs.setValue(#tag, (b) );

#define WRITE_STRING_SETTING(prefs, prefix, var)\
  prefs.setValue(#var , this->prefix##var->text());

#define WRITE_SETTING(prefs, prefix, var)\
  prefs.setValue(#var, this->prefix##var->value());


#define WRITE_CHECKBOX_SETTING(prefs, prefix, o)\
  prefs.setValue( #o, prefix##o->isChecked() );

#define WRITE_COMBO_BOX(prefs, prefix, combobox)\
  prefs.setValue(#combobox, this->prefix##combobox->currentIndex());


#define READ_COMBO_BOX(prefs, prefix, combobox)\
  { bool ok = false;\
    int i = prefs.value(#combobox).toInt(&ok);\
    if (false == ok) {i=0;}\
    prefix##combobox->setCurrentIndex(i); }

#define WRITE_INT_SETTING(prefs, var)\
  prefs.setValue(#var, var);

#define READ_INT_SETTING(prefs, var, def)\
  { bool ok = false;\
    var = prefs.value(#var).toInt(&ok);\
    if (false == ok) {i=def;} }


#define CHECK_QLABEL_OUTPUT_FILE_EXISTS_ICON_BODY1(ns, prefixname, name)\
  prefixname->setText(m_OutputFilePrefix->text() + ns::name.c_str());\
  prefixname##Icon->setPixmap(QPixmap(iconFile));\

#define CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefixname, name)\
  prefixname->setText(m_OutputFilePrefix->text() + ns::name.c_str());



#define CHECK_QLINEEDIT_FILE_EXISTS(name) \
  { \
    QString absPath = QDir::toNativeSeparators(name->text());\
    QFileInfo fi ( absPath );\
    QString iconFile;\
    if ( fi.exists() && fi.isFile() )  {\
      iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
    } else {\
      iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
    }\
    name##Icon->setPixmap(QPixmap(iconFile));\
  }


#define CHECK_QLABEL_OUTPUT_FILE_EXISTS(ns, prefix, name) \
  { \
    QString absPath = prefix##OutputDir->text() + QDir::separator() + m_OutputFilePrefix->text() + ns::name.c_str();\
    absPath = QDir::toNativeSeparators(absPath);\
    QFileInfo fi ( absPath );\
    CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefix##name, name)\
  }


#define CHECK_QLABEL_OUTPUT_FILE_EXISTS_ICON(ns, prefix, name) \
  { \
    QString absPath = prefix##OutputDir->text() + QDir::separator() + m_OutputFilePrefix->text() + ns::name.c_str();\
    absPath = QDir::toNativeSeparators(absPath);\
    QFileInfo fi ( absPath );\
    CHECK_QLABEL_OUTPUT_FILE_EXISTS_ICON_BODY1(ns, prefix##name, name)\
  }


#define CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(ns, prefix, name) \
  { \
    QString absPath = prefix##OutputDir->text() + QDir::separator() + m_OutputFilePrefix->text() + ns::name.c_str();\
    absPath = QDir::toNativeSeparators(absPath);\
    QFileInfo fi ( absPath );\
    CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefix##name, name)\
  }


#define SANITY_CHECK_INPUT(prefix, input)\
  if (verifyPathExists(prefix##input->text(), prefix##input) == false) {\
    QMessageBox::critical(this, tr("DREAM.3D"),\
                          tr("The input " #input " does not exist. Please ensure the file or folder exists before starting the operation"),\
                          QMessageBox::Ok,\
                          QMessageBox::Ok);\
    return;\
  }


#define SANITY_CHECK_QLABEL_FILE(ns, prefix, input) \
  { \
    QString absPath = prefix##InputDir->text() + QDir::separator() + ns::input.c_str();\
    absPath = QDir::toNativeSeparators(absPath);\
    QFileInfo fi ( absPath );\
    QString theMessage = QString("The input ") + QString(ns::input.c_str()) + \
                         QString(" does not exist. Please ensure the file or folder exists before starting the operation");\
    if ( fi.exists() == false)  {\
      QMessageBox::critical(this, tr("DREAM.3D"),\
                            theMessage,\
                            QMessageBox::Ok,\
                            QMessageBox::Ok);\
      return;\
    }\
  }

#define CHECK_QLABEL_INPUT_FILE_EXISTS(ns, prefix, name) \
  { \
    QString absPath = prefix##InputDir->text() + QDir::separator() + ns::name.c_str();\
    absPath = QDir::toNativeSeparators(absPath);\
    QFileInfo fi ( absPath );\
    QString iconFile;\
    if ( fi.exists() )  {\
      iconFile = QString(":/") + QString("Check") + QString("-16x16.png");\
    } else {\
      iconFile = QString(":/") + QString("Delete") + QString("-16x16.png");\
    }\
    CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefix##name, name)\
  }



#endif /* AIM_QTMACROS_H_ */

