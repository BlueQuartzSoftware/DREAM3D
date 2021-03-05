#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

class AbstractFilter;

class PreflightVerify : public QObject
{
  Q_OBJECT
public:
  PreflightVerify(QObject* parent = nullptr);
  virtual ~PreflightVerify();

  bool m_widgetChanged = {false};
  bool m_beforePreflight = {false};
  bool m_afterPreflight = {false};
  bool m_filterNeedsInputParameters = {false};

public Q_SLOTS:
  void widgetChanged(const QString& msg);
  void beforePreflight();
  void afterPreflight();
  void filterNeedsInputParameters(AbstractFilter* filter);

private:
};
