#ifndef _progress_dialog_H_
#define _progress_dialog_H_


#include <QtWidgets/QDialog>

#include "ui_ProgressDialog.h"

class ProgressDialog : public QDialog, private Ui::ProgressDialog
{
  Q_OBJECT

  public:
    ProgressDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~ProgressDialog();

    void setLabelText(const QString& text);

  protected:


  private:

};

#endif /* _progress_dialog_H_ */
