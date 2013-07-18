

#include "QtFilter.h"


class SegmentGrainsData : public QSharedData
{
  public:
    SegmentGrainsData() : QSharedData()
    { }

    SegmentGrainsData(const SegmentGrainsData &other)
      : QSharedData(other)
    { }

    ~SegmentGrainsData() { }

    float m_Misorientation;
    QString m_InputFile;
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentGrains::SegmentGrains()
{
  d_ptr = new SegmentGrainsData;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SegmentGrains::SegmentGrains(const SegmentGrains &other)
  : d_ptr (other.d_ptr)
{
}


DREAM3D_FILTER_PROPERTY_DEFN(float, Misorientation, SegmentGrains)

DREAM3D_FILTER_PROPERTY_DEFN(QString, InputFile, SegmentGrains)

