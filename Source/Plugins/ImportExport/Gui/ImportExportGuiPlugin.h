#pragma once

#include "ImportExport/ImportExportPlugin.h"

class ImportExportGuiPlugin : public ImportExportPlugin
{
  Q_OBJECT
  Q_INTERFACES(ISIMPLibPlugin)
  Q_PLUGIN_METADATA(IID "com.your.domain.ImportExportGuiPlugin")

public:
  ImportExportGuiPlugin();
  ~ImportExportGuiPlugin() override;

  /**
   * @brief Register all the filters with the FilterWidgetFactory
   */
  void registerFilterWidgets(FilterWidgetManager* fwm) override;

public:
  ImportExportGuiPlugin(const ImportExportGuiPlugin&) = delete;            // Copy Constructor Not Implemented
  ImportExportGuiPlugin(ImportExportGuiPlugin&&) = delete;                 // Move Constructor
  ImportExportGuiPlugin& operator=(const ImportExportGuiPlugin&) = delete; // Copy Assignment Not Implemented
  ImportExportGuiPlugin& operator=(ImportExportGuiPlugin&&) = delete;      // Move Assignment Not Implemented
};
