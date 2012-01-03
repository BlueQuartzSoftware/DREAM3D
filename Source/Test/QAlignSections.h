/*
 * QAlignSections.h
 *
 *  Created on: Dec 30, 2011
 *      Author: mjackson
 */

#ifndef QALIGNSECTIONS_H_
#define QALIGNSECTIONS_H_

#include <QtCore/QObject>

#include "FilterWidgets/QFilterWidget.h"
#include "DREAM3DLib/ReconstructionFilters/AlignSections.h"
#include "DREAM3DLib/ReconstructionFilters/SegmentGrains.h"


class QAlignSections : public QObject, public AlignSections
{
    Q_OBJECT;
  public:
    QAlignSections() : QObject(), AlignSections(){}
    virtual ~QAlignSections(){}


    Q_PROPERTY(float misorientationtolerance READ getmisorientationtolerance WRITE setmisorientationtolerance)

};

//class QSegmentGrains : public QObject
//{
//    Q_OBJECT;
//  public:
//    QSegmentGrains() : QObject(), SegmentGrains(){
//      m_Filter = SegmentGrains::New();
//      setObjectName("SegmentGrains");
//    }
//    virtual ~QSegmentGrains(){}
//
//
//
//
//  private:
//
//};

class QSegmentGrainsWidget : public QFilterWidget
{
  Q_OBJECT;
  public:
    QSegmentGrainsWidget(QWidget* parent = NULL):
      QFilterWidget(parent)
    {
      m_Filter = SegmentGrains::New();
      setupGui();
      setTitle("Segment Grains");
    }
    virtual ~QSegmentGrainsWidget(){}

    Q_PROPERTY(float MisoTolerance READ getMisoTolerance WRITE setMisoTolerance)

    void setMisoTolerance(float v)
    {
      m_Filter->setMisoTolerance(v);
    }
    float getMisoTolerance()
    {
      return m_Filter->getMisoTolerance();
    }

    std::vector<FilterOption::Pointer> getFilterOptions() {
      return m_Filter->getFilterOptions();
    }

  private:
    SegmentGrains::Pointer m_Filter;
};


#endif /* QALIGNSECTIONS_H_ */
