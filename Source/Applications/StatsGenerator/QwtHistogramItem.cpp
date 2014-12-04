
#include "QwtHistogramItem.h"



#include <QtCore/QString>
#include <QtGui/QPainter>


#include <qwt_plot.h>
#include <qwt_interval_data.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>


class QwtHistogramItem::PrivateData
{
public:
    int attributes;
    QwtIntervalData data;
    QColor color;
    double reference;
};

QwtHistogramItem::QwtHistogramItem(const QwtText &title):
    QwtPlotItem(title)
{
    init();
}

QwtHistogramItem::QwtHistogramItem(const QString &title):
    QwtPlotItem(QwtText(title))
{
    init();
}

QwtHistogramItem::~QwtHistogramItem()
{
    delete d_data;
}

void QwtHistogramItem::init()
{
    d_data = new PrivateData();
    d_data->reference = 0.0;
    d_data->attributes = QwtHistogramItem::Auto;

    setItemAttribute(QwtPlotItem::AutoScale, true);
    setItemAttribute(QwtPlotItem::Legend, true);

    setZ(20.0);
}

void QwtHistogramItem::setBaseline(double reference)
{
    if ( d_data->reference != reference )
    {
        d_data->reference = reference;
        itemChanged();
    }
}

double QwtHistogramItem::baseline() const
{
    return d_data->reference;
}

void QwtHistogramItem::setData(const QwtIntervalData &data)
{
    d_data->data = data;
    itemChanged();
}

const QwtIntervalData &QwtHistogramItem::data() const
{
    return d_data->data;
}

void QwtHistogramItem::setColor(const QColor &color)
{
    if ( d_data->color != color )
    {
        d_data->color = color;
        itemChanged();
    }
}

QColor QwtHistogramItem::color() const
{
    return d_data->color;
}

QwtDoubleRect QwtHistogramItem::boundingRect() const
{
    QwtDoubleRect rect = d_data->data.boundingRect();
    if ( !rect.isValid() )
        return rect;

    if ( d_data->attributes & Xfy )
    {
        rect = QwtDoubleRect( rect.y(), rect.x(),
            rect.height(), rect.width() );

        if ( rect.left() > d_data->reference )
            rect.setLeft( d_data->reference );
        else if ( rect.right() < d_data->reference )
            rect.setRight( d_data->reference );
    }
    else
    {
        if ( rect.bottom() < d_data->reference )
            rect.setBottom( d_data->reference );
        else if ( rect.top() > d_data->reference )
            rect.setTop( d_data->reference );
    }

    return rect;
}


int QwtHistogramItem::rtti() const
{
    return QwtPlotItem::Rtti_PlotHistogram;
}

void QwtHistogramItem::setHistogramAttribute(HistogramAttribute attribute, bool on)
{
    if ( bool(d_data->attributes & attribute) == on )
        return;

    if ( on )
        d_data->attributes |= attribute;
    else
        d_data->attributes &= ~attribute;

    itemChanged();
}

bool QwtHistogramItem::testHistogramAttribute(HistogramAttribute attribute) const
{
    return d_data->attributes & attribute;
}

void QwtHistogramItem::draw(QPainter *painter, const QwtScaleMap &xMap,
    const QwtScaleMap &yMap, const QRect &) const
{
    const QwtIntervalData &iData = d_data->data;

    painter->setPen(QPen(d_data->color));

    const int x0 = xMap.transform(baseline());
    const int y0 = yMap.transform(baseline());

    for ( int i = 0; i < (int)iData.size(); i++ )
    {
        if ( d_data->attributes & QwtHistogramItem::Xfy )
        {
            const int x2 = xMap.transform(iData.value(i));
            if ( x2 == x0 )
                continue;

            int y1 = yMap.transform( iData.interval(i).minValue());
            int y2 = yMap.transform( iData.interval(i).maxValue());
            if ( y1 > y2 )
                qSwap(y1, y2);

            if ( i < (int)iData.size() - 2 )
            {
                const int yy1 = yMap.transform(iData.interval(i+1).minValue());
                const int yy2 = yMap.transform(iData.interval(i+1).maxValue());

                if ( y2 == qwtMin(yy1, yy2) )
                {
                    const int xx2 = xMap.transform(
                        iData.interval(i+1).minValue());
                    if ( xx2 != x0 && ( (xx2 < x0 && x2 < x0) ||
                                          (xx2 > x0 && x2 > x0) ) )
                    {
                       // One pixel distance between neighboured bars
                       y2++;
                    }
                }
            }

            drawBar(painter, Qt::Horizontal,
                QRect(x0, y1, x2 - x0, y2 - y1));
        }
        else
        {
            const int y2 = yMap.transform(iData.value(i));
            if ( y2 == y0 )
                continue;

            int x1 = xMap.transform(iData.interval(i).minValue());
            int x2 = xMap.transform(iData.interval(i).maxValue());
            if ( x1 > x2 )
                qSwap(x1, x2);

            if ( i < (int)iData.size() - 2 )
            {
                const int xx1 = xMap.transform(iData.interval(i+1).minValue());
                const int xx2 = xMap.transform(iData.interval(i+1).maxValue());

                if ( x2 == qwtMin(xx1, xx2) )
                {
                    const int yy2 = yMap.transform(iData.value(i+1));
                    if ( yy2 != y0 && ( (yy2 < y0 && y2 < y0) ||
                                    (yy2 > y0 && y2 > y0) ) )
                    {
                        // One pixel distance between neighboured bars
                        x2--;
                    }
                }
            }
            drawBar(painter, Qt::Vertical,
                QRect(x1, y0, x2 - x1, y2 - y0) );
        }
    }
}

void QwtHistogramItem::drawBar(QPainter *painter,
   Qt::Orientation, const QRect& rect) const
{
   painter->save();

   const QColor color(painter->pen().color());
#if QT_VERSION >= 0x040000
   const QRect r = rect.normalized();
#else
   const QRect r = rect.normalize();
#endif

   const int factor = 125;
   const QColor light(color.light(factor));
   const QColor dark(color.dark(factor));

   painter->setBrush(color);
   painter->setPen(Qt::NoPen);
   QwtPainter::drawRect(painter, r.x() + 1, r.y() + 1,
      r.width() - 2, r.height() - 2);
   painter->setBrush(Qt::NoBrush);

   painter->setPen(QPen(light, 2));
#if QT_VERSION >= 0x040000
   QwtPainter::drawLine(painter,
      r.left() + 1, r.top() + 2, r.right() + 1, r.top() + 2);
#else
   QwtPainter::drawLine(painter,
      r.left(), r.top() + 2, r.right() + 1, r.top() + 2);
#endif

   painter->setPen(QPen(dark, 2));
#if QT_VERSION >= 0x040000
   QwtPainter::drawLine(painter,
      r.left() + 1, r.bottom(), r.right() + 1, r.bottom());
#else
   QwtPainter::drawLine(painter,
      r.left(), r.bottom(), r.right() + 1, r.bottom());
#endif

   painter->setPen(QPen(light, 1));

#if QT_VERSION >= 0x040000
   QwtPainter::drawLine(painter,
      r.left(), r.top() + 1, r.left(), r.bottom());
   QwtPainter::drawLine(painter,
      r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 1);
#else
   QwtPainter::drawLine(painter,
      r.left(), r.top() + 1, r.left(), r.bottom() + 1);
   QwtPainter::drawLine(painter,
      r.left() + 1, r.top() + 2, r.left() + 1, r.bottom());
#endif

   painter->setPen(QPen(dark, 1));

#if QT_VERSION >= 0x040000
   QwtPainter::drawLine(painter,
      r.right() + 1, r.top() + 1, r.right() + 1, r.bottom());
   QwtPainter::drawLine(painter,
      r.right(), r.top() + 2, r.right(), r.bottom() - 1);
#else
   QwtPainter::drawLine(painter,
      r.right() + 1, r.top() + 1, r.right() + 1, r.bottom() + 1);
   QwtPainter::drawLine(painter,
      r.right(), r.top() + 2, r.right(), r.bottom());
#endif

   painter->restore();
}
