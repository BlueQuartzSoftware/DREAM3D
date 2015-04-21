/*
 * PMFilterGenerator.h
 *
 *  Created on: Jun 7, 2012
 *      Author: mjackson
 */

#ifndef _PMFILTERGENERATOR_H_
#define _PMFILTERGENERATOR_H_

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

	virtual QString generateFileContents();

  public slots:
  virtual void generateOutput();

  protected slots:
    virtual void pluginNameChanged (const QString& plugname);
    virtual void outputDirChanged (const QString& outputDir);
	virtual void generateTestFileLocationsOutput(QSet<QString> names);

  signals:
    void filterSourceError(const QString& message);


  private:
    QString m_ClassName;


};

#endif /* PMFILTERGENERATOR_H_ */

