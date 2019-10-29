#pragma once

#include <memory>

#include <cstdio>

#include <iostream>

#include <QtCore/QTextStream>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SIMPLib/DataContainers/DataContainerArray.h"

class DataContainerArray;
using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

static const QString DCName("Orientation Transforms Test");
static const QString AMName("Angles");

QString k_InputNames[7] = {"eu", "om", "qu", "ax", "ro", "ho", "cu"};
int k_CompDims[7] = {3, 9, 4, 4, 4, 3, 3};

namespace OrientationPrinters
{

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void Print_EU(const T& om)
{
  printf("EU:% 3.16f % 3.16f % 3.16f\n", om[0], om[1], om[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void Print_OM(const T& om)
{
  printf("OM: /    % 3.16f    % 3.16f    % 3.16f    \\\n", om[0], om[1], om[2]);
  printf("OM: |    % 3.16f    % 3.16f    % 3.16f    |\n", om[3], om[4], om[5]);
  printf("OM: \\    % 3.16f    % 3.16f    % 3.16f    /\n", om[6], om[7], om[8]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void Print_AX(const T& om)
{
  printf("Ax:<% 3.16f % 3.16f % 3.16f>% 3.16f\n", om[0], om[1], om[2], om[3]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K> void Print_RO(const T& om)
{
  // if(om[3] != std::numeric_limits<K>::infinity())
  //  printf("Rodrigues vector                 :   % 3.6f    % 3.6f    % 3.6f\n", om[0], om[1], om[2] );
  // else
  printf("Ro:% 3.16f % 3.16f % 3.16f % 3.16f\n", om[0], om[1], om[2], om[3]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K> void Print_HO(const T& om)
{
  printf("Ho:% 3.16f % 3.16f % 3.16f\n", om[0], om[1], om[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T, typename K>
void Print_QU(const T& om, typename Quaternion<K>::Order layout = Quaternion<K>::Order::VectorScalar)
{
  if(layout == Quaternion<K>::Order::VectorScalar)
  {
    printf("QU:<% 3.16f % 3.6f % 3.16f> % 3.16f\n", om[0], om[1], om[2], om[3]);
  }

  else if(layout == Quaternion<K>::Order::ScalarVector)
  {
    printf("QU: % 3.16f <% 3.16f % 3.16f % 3.16f>\n", om[0], om[1], om[2], om[3]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void Print_HO(const T& om)
{
  printf("Ho:% 3.16f % 3.16f % 3.16f\n", om[0], om[1], om[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T> void Print_CU(const T& om)
{
  printf("Cu:% 3.16f % 3.16f % 3.16f\n", om[0], om[1], om[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename K>
void PrintTuple(DataContainerArrayShPtrType dca, size_t t)
{
  for(int i = 0; i < 7; i++)
  {
    DataArrayPath daPath(DCName, AMName, k_InputNames[i]);
    std::vector<size_t> cDims(1, k_CompDims[i]);
    typename DataArray<K>::Pointer data = dca->getPrereqArrayFromPath<DataArray<K>, AbstractFilter>(nullptr, daPath, cDims);
    std::cout << "'" << data->getName().toStdString() << "'"
              << "\t";
    QString text;
    QTextStream ss(&text);
    data->printTuple(ss, t);
    std::cout << text.toStdString() << std::endl;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename K> void PrintTuple(typename DataArray<K>::Pointer data, size_t t)
{
  std::vector<size_t> cDims = data->getComponentDimensions();
  printf("%s\n", data->getName().toStdString().c_str());
  for(int a = 0; a < cDims[0]; a++)
  {
    printf("% 3.16f", data->getComponent(t, a));
    if(a < cDims[0] - 1)
    {
      printf("  ");
    }
    if(cDims[0] == 9 && (a == 2 || a == 5))
    {
      printf("\n");
    }
  }
  printf("\n");
}
}

