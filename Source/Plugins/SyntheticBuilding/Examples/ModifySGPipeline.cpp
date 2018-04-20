

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QString>

#include "StatsGeneratorExample.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getInputFile()
{
  return StatsGen::Example::PipelineFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString getOutputDir()
{
  return StatsGen::Example::OutputDir;
}

/**
 * @brief Reads the Pipeline file into a QByteArray
 * @param inputFile
 * @return
 */
QByteArray readFile(const QString& inputFile)
{

  QFileInfo fi(inputFile);
  if(!fi.exists())
  {
    qDebug() << "Input file '" << inputFile << "' does not exist.";
    return QByteArray();
  }

  QFile source(inputFile);
  if(!source.open(QFile::ReadOnly))
  {
    qDebug() << "Error opening '" << inputFile << "' for reading";
    return QByteArray();
  }

  QByteArray contents = source.readAll();
  source.close();
  return contents;
}

/**
 * @brief Converts the QByteArray into a QJsonDocument
 * @param json
 * @return
 */
QJsonDocument parseInputFile(const QByteArray& json)
{

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(json, &error);
  if(error.error != QJsonParseError::NoError)
  {
    qDebug() << "Error Parsing Pipeline Json. Reported error was: ";
    qDebug() << error.errorString();
    return QJsonDocument();
  }

  return doc;
}

/**
 * @brief findNumberOfFilters
 * @param root
 * @return The number of filters in the pipeline or -1 if there was an error
 */
int findNumberOfFilters(const QJsonObject& root)
{
  QJsonValue pipelineBuilderObj = root["PipelineBuilder"];
  if(pipelineBuilderObj.isUndefined())
  {
    return -1;
  }
  QJsonValue numberFilters = pipelineBuilderObj.toObject().value("Number_Filters");
  if(!numberFilters.isUndefined())
  {
    return numberFilters.toInt(-1);
  }
  return -1;
}

/**
 * @brief Returns the index into the pipeline that represents the first instance
 * of the StatsGeneratorFilter.
 * @param root
 * @param numFilters
 * @return The index of the StatsGeneratorFilter in the pipeline or -1 if it is not found.
 */
int findStatsGenFilter(const QJsonObject& root, int numFilters)
{
  int index = -1;
  for(int i = 0; i < numFilters; i++)
  {
    QJsonValue filter = root[QString::number(i)];
    if(filter.isObject())
    {
      QString filterName = filter.toObject().value("Filter_Name").toString();
      if(!filterName.isEmpty() && filterName.compare("StatsGeneratorFilter") == 0)
      {
        return i;
      }
    }
  }

  return index;
}

/**
 Example JSON for a part of a StatsGeneratorFilter json representation just
 showing the important part that we are interested in.
 @code

{
    "0": {
        "CellEnsembleAttributeMatrixName": "CellEnsembleData",
        "CrystalStructuresArrayName": "CrystalStructures",
        "Filter_Human_Label": "StatsGenerator",
        "Filter_Name": "StatsGeneratorFilter",
        "PhaseTypesArrayName": "PhaseTypes",
        "StatsDataArray": {
            "1": {
                "FeatureSize Distribution": {
                    "Average": 1,
                    "Standard Deviation": 0.10000000149011612
                },
            }
        }
  }

  @endcode
*/

/**
 * @brief Digs into the proper Json object, edits the value and winds itself back out
 * @param root The root of the Json tree
 * @param index The index of the filter of interest
 * @param phase The phase of interest
 * @param avgSize The new value to set into the json
 */
void adjustAverageGrainSize(QJsonObject& root, int index, int phase, float avgSize)
{
  // Drill down through the filter index to get the Json Object for the filter
  QJsonObject filterObj = root[QString::number(index)].toObject();
  // Then to the StatsDataArray Json Object
  QJsonObject statsDataArrayObj = filterObj["StatsDataArray"].toObject();
  // Then to the proper phase index Json Object
  QJsonObject phaseObj = statsDataArrayObj.value(QString::number(phase)).toObject();
  // Then to the Feature Size Distribution Json object
  QJsonObject featureSizeDistObj = phaseObj["FeatureSize Distribution"].toObject();
  // Set the proper Average Grain Size..
  featureSizeDistObj["Average"] = avgSize;
  // The "set" the edited Json Objects back into the proper places back up the chain.
  QJsonObject::Iterator iter = phaseObj.insert("FeatureSize Distribution", featureSizeDistObj);

  iter = statsDataArrayObj.insert(QString::number(phase), phaseObj);

  iter = filterObj.insert("StatsDataArray", statsDataArrayObj);

  iter = root.insert(QString::number(index), filterObj);
  // and finally return.
}

/**
 * @brief Writes the Json back out to a file based on the index to the designated
 * output directory. For this example the output directory is in the build directory
 * of the project.
 * @param root
 * @param index
 */
void writePipelineFile(const QJsonObject& root, const QString index)
{
  QString filePath = getOutputDir() + QDir::separator() + "PipelineFile-" + index + ".json";
  QFile file(filePath);
  file.open(QFile::WriteOnly);
  QTextStream stream(&file);
  stream << QJsonDocument(root).toJson();
  file.close();

  qDebug() << filePath;
}

/**
 * @brief This example program will generate 4 DREAM.3D pipelines based on an example Pipeline File
 * that is already in the project. The code will edit the "Average" value in the "FeatureSize Distribution"
 * of Phase=1.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  QByteArray json = readFile(getInputFile());
  QJsonDocument jsonDoc = parseInputFile(json);

  // The synthetic only has 1 phase
  int phaseIndex = 1;
  // Start at an average size of 1.0 and increment by 0.5 with each loop
  float avgSize = 1.0f;
  for(int i = 0; i < 4; i++)
  {

    QJsonObject root = jsonDoc.object();

    // Pipeline files have top level objects numbered from 0->n. One can get the value
    // of "n" by inspecting the top level "PipelineBuilder" object and get the "Number_Filters"
    // integer value.
    int numFilters = findNumberOfFilters(root);
    if(numFilters < 1)
    {
      return 1;
    }

    // Now we need to find the filter index that is the "StatsGeneratorFilter" by using the "Filter_Name"
    // key that is in each numbered entry of the pipeline file.
    int filterIndex = findStatsGenFilter(root, numFilters);

    // Now actually edit the value in the Json representation
    adjustAverageGrainSize(root, filterIndex, phaseIndex, avgSize);

    // Write the edited file back out to a file.
    writePipelineFile(root, QString::number(i));

    // Increment the Average Value for the next loop.
    avgSize += 0.5;
  }

  return 0;
}
