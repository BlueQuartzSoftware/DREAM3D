/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef AIM_QTMACROS_H_
#define AIM_QTMACROS_H_


#define READ_CHECKBOX_SETTING(prefs, tag, emptyValue)\
  { QString s = prefs.value(#tag).toString();\
  if (s.isEmpty() == false) {\
    bool bb = prefs.value(#tag).toBool();\
  tag->setChecked(bb); } else { tag->setChecked(emptyValue); } }


#define WRITE_CHECKBOX_SETTING(prefs, o)\
    prefs.setValue( o->objectName(), o->isChecked() );



#define READ_STRING_SETTING(prefs, var, emptyValue)\
  var->setText( prefs.value(#var).toString() );\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }

#define READ_FILEPATH_SETTING(prefs, var, emptyValue)\
    var->blockSignals(true);\
  var->setText( prefs.value(#var).toString() );\
  verifyPathExists(prefs.value(#var).toString(), var);\
  if (var->text().isEmpty() == true) { var->setText(emptyValue); }\
  var->blockSignals(false);

#define READ_SETTING(prefs, var, ok, temp, default, type)\
  ok = false;\
  temp = prefs.value(#var).to##type(&ok);\
  if (false == ok) {temp = default;}\
  var->setValue(temp);

#define READ_BOOL_SETTING(prefs, prefix, tag, emptyValue)\
  { QString s = prefs.value(#tag).toString();\
  if (s.isEmpty() == false) {\
    bool bb = prefs.value(#tag).toBool();\
  prefix##tag->setChecked(bb); } else { prefix##tag->setChecked(emptyValue); } }

#define WRITE_BOOL_SETTING(prefs, tag,  b)\
    prefs.setValue(#tag, (b) );

#define WRITE_STRING_SETTING(prefs, var)\
  prefs.setValue(#var , this->var->text());

#define WRITE_SETTING(prefs, var)\
  prefs.setValue(#var, this->var->value());


#define READ_COMBO_BOX(prefs, combobox)\
    { bool ok = false;\
    int i = prefs.value(#combobox).toInt(&ok);\
    if (false == ok) {i=0;}\
    combobox->setCurrentIndex(i); }

#define WRITE_INT_SETTING(prefs, var)\
    prefs.setValue(#var, var);

#define READ_INT_SETTING(prefs, var, def)\
    { bool ok = false;\
    var = prefs.value(#var).toInt(&ok);\
    if (false == ok) {i=def;} }

#define WRITE_COMBO_BOX(prefs, combobox)\
  prefs.setValue(#combobox, this->combobox->currentIndex());


#define CHECK_QLABEL_OUTPUT_FILE_EXISTS_BODY1(ns, prefixname, name)\
  prefixname->setText(ns::name.c_str());\
  prefixname##Icon->setPixmap(QPixmap(iconFile));\



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
  QString absPath = prefix##OutputDir->text() + QDir::separator() + ns::name.c_str();\
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


#define CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(ns, prefix, name) \
{ \
  QString absPath = prefix##OutputDir->text() + QDir::separator() + ns::name.c_str();\
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


#define SANITY_CHECK_INPUT(prefix, input)\
  if (verifyPathExists(prefix##input->text(), prefix##input) == false) {\
  QMessageBox::critical(this, tr("AIM Representation"),\
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
  QMessageBox::critical(this, tr("AIM Representation"),\
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
