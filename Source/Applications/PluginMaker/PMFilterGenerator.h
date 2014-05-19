/*
 * PMFilterGenerator.h
 *
 *  Created on: Jun 7, 2012
 *      Author: mjackson
 */

#ifndef PMFILTERGENERATOR_H_
#define PMFILTERGENERATOR_H_

#include "PMFileGenerator.h"

/*
 *
 */
class PMFilterGenerator : public PMFileGenerator
{
    Q_OBJECT

  public:
    PMFilterGenerator(QString outputDir,
                      QString pathTemplate,
                      QString fileName,
                      QString className,
                      QString codeTemplateResourcePath,
                      QTreeWidgetItem* wi,
                      QObject* parent = 0);
    virtual ~PMFilterGenerator();

  protected slots:
    virtual void pluginNameChanged (const QString& plugname);
    virtual void outputDirChanged (const QString& outputDir);
    virtual void generateOutput();

  signals:
    void outputError(const QString& message);
    void filterSourceError(const QString& message);


  private:
    QString m_ClassName;


};

#endif /* PMFILTERGENERATOR_H_ */

