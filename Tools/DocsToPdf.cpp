/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QDebug>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QUrl>


#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>
#include <QtGui/QTextBrowser>
#include <QtGui/QPrintEngine>
#include <QtGui/QAbstractTextDocumentLayout>



#include "DocsToPdf.h"


static void printPage(int index, QPainter *painter, const QTextDocument *doc, const QRectF &body, const QPointF &pageNumberPos)
{
  painter->save();
  painter->translate(body.left(), body.top() - (index - 1) * body.height());
  QRectF view(0, (index - 1) * body.height(), body.width(), body.height());

  QAbstractTextDocumentLayout *layout = doc->documentLayout();
  QAbstractTextDocumentLayout::PaintContext ctx;

  painter->setClipRect(view);
  ctx.clip = view;

  // don't use the system palette text as default text color, on HP/UX
  // for example that's white, and white text on white paper doesn't
  // look that nice
  ctx.palette.setColor(QPalette::Text, Qt::black);

  layout->draw(painter, ctx);

  //    if (!pageNumberPos.isNull()) {
  //        painter->setClipping(false);
  //        painter->setFont(QFont(doc->defaultFont()));
  //        const QString pageString = QString::number(index);

  //        painter->drawText(qRound(pageNumberPos.x() - painter->fontMetrics().width(pageString)),
  //                          qRound(pageNumberPos.y() + view.top()),
  //                          pageString);
  //    }

  //    painter->restore();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void printFilterDirectory(QDir filterDir, QPrinter* printer, QPainter* painter, QString &html)
{
  qDebug() << "  Dir " << filterDir.path();
  QTextBrowser browser;

  foreach (QString fileName, filterDir.entryList(QDir::Files))
  {
    if(fileName.endsWith(".html", Qt::CaseSensitive))
    {

      QFile file(filterDir.path() + "/" + fileName);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
      QTextStream in(&file);
      html = html + in.readAll();
      file.close();
#if 0
      bool success = printer->newPage();
      if (false == success)
      {
        qDebug() << "Printer State: " << printer->printerState();
        return;
      }
      QUrl url("file:///" + filterDir.path() + "/" + fileName);
      qDebug() << "  Printing:" << url;
      browser.setSource(url);
      browser.render(painter);
      //browser.print(printer);
#endif
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QPrinter* printer = new QPrinter(QPrinter::ScreenResolution);
  printer->setOutputFormat(QPrinter::PdfFormat);
  printer->setOutputFileName(DocsToPdf::GetDocsPdfOutputFilename());
  printer->setPrintRange(QPrinter::AllPages);

  QPainter painter;
  // painter.begin(printer);

  QDir filterDir = QDir(DocsToPdf::FilterDirectory());
  QTextDocument doc;
  QString html;
  foreach (QString fileName, filterDir.entryList(QDir::Files))
  {
    if(fileName.endsWith(".html", Qt::CaseSensitive))
    {

#if 0
      QUrl url("file:///" + filterDir.path() + "/" + fileName);
      browser.loadResource(QTextDocument::HtmlResource, url);
      browser.render(&painter);
#else
      QFile file(filterDir.path() + "/" + fileName);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return EXIT_FAILURE;
      QTextStream in(&file);
      html = in.readAll();
      file.close();
#if 0
      // qDebug() << "Printing:" << filterDir.path() << "/" << fileName;
      doc.setHtml(html);

      QRect rect = printer->pageRect();
      doc.setTextWidth(rect.width());
      int count = doc.pageCount();
      //doc.drawContents(&painter);
      doc.print(printer);
      printer->newPage();
      //doc.print(printer);
      //doc.drawContents(&painter);
#endif
#endif
    }
  }

  //if (true) {   painter.end(); return EXIT_FAILURE; }

  filterDir.entryList(QDir::Dirs);
  foreach(QString filterPath, filterDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
  {
    filterDir.cd(filterPath);
    printFilterDirectory(filterDir, printer, &painter, html);
    filterDir.cdUp();
  }

  doc.setHtml(html);
  QRect rect = printer->pageRect();
  doc.setTextWidth(rect.width());
  int count = doc.pageCount();
  doc.print(printer);

  return 0;
}
