#ifndef _PLUGINMAKER_H_
#define _PLUGINMAKER_H_

#include "ui_PluginMaker.h"
#include <QtGui/QFileDialog>
#include <QtGui/QDialog>

class PluginMaker : public QFileDialog, public Ui::PluginMaker {
  Q_OBJECT

public:
  PluginMaker(QWidget* parent = 0);

private slots:
  void on_selectButton_clicked();
};

#endif