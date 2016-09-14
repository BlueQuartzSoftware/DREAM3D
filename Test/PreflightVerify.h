#ifndef _preflightverify_h_
#define _preflightverify_h_



#include <QtCore/QObject>
#include <QtCore/QString>



class AbstractFilter;

class PreflightVerify : public QObject
{
    Q_OBJECT
  public:
    PreflightVerify(QObject* parent = nullptr);
    virtual ~PreflightVerify();


    bool m_widgetChanged;
    bool m_beforePreflight;
    bool m_afterPreflight;
    bool m_filterNeedsInputParameters;


  public slots:
    void widgetChanged(const QString& msg);
    void beforePreflight();
    void afterPreflight();
    void filterNeedsInputParameters(AbstractFilter* filter);
  private:

};


#endif /* _PreflightVerify_H_ */
