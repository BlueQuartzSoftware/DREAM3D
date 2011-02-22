
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorFunc::switchOrientations( int &badtrycount, int &numbins)
{
  // double random = rg.Random();
  int good = 0;
  double deltaerror;
  int selectedgrain1;
  int selectedgrain2;
  double q1[5];
  double qref[5];
  qref[1] = SinOfZero * CosOfZero; // sin(0.5 * 0.0) * cos(0.5 * (0.0 - 0.0));
  qref[2] = SinOfZero * SinOfZero; // sin(0.5 * 0.0) * sin(0.5 * (0.0 - 0.0));
  qref[3] = CosOfZero * SinOfZero; // cos(0.5 * 0.0) * sin(0.5 * (0.0 + 0.0));
  qref[4] = CosOfZero * CosOfZero; // cos(0.5 * 0.0) * cos(0.5 * (0.0 + 0.0));

  double dim1 = std::numeric_limits<double >::max(), dim2 = std::numeric_limits<double >::max(), dim3 = std::numeric_limits<double >::max();

  int g1odfbin = std::numeric_limits<int >::max(), g2odfbin = std::numeric_limits<int >::max();

  double g1ea1 = std::numeric_limits<double >::max(), g1ea2 = std::numeric_limits<double >::max(), g1ea3 = std::numeric_limits<double >::max(), g2ea1 =
      std::numeric_limits<double >::max(), g2ea2 = std::numeric_limits<double >::max(), g2ea3 = std::numeric_limits<double >::max();

  IntVectorType nlist;
  vector<double>* misolist;
  DoubleVectorType neighborsurfarealist;

  MisorientationCalculations::initializeDims(crystruct, dim1, dim2, dim3, numbins);

  good = 0;
  while (good == 0)
  {
    selectedgrain1 = int(rg.Random() * numgrains);
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == numgrains) selectedgrain1 = numgrains - 1;
    selectedgrain2 = int(rg.Random() * numgrains);
    if (selectedgrain2 == 0) selectedgrain2 = 1;
    if (selectedgrain2 == numgrains) selectedgrain2 = numgrains - 1;
    if (m_Grains[selectedgrain1]->surfacegrain == 0 && m_Grains[selectedgrain2]->surfacegrain == 0) good = 1;
  }
  g1ea1 = m_Grains[selectedgrain1]->euler1;
  g1ea2 = m_Grains[selectedgrain1]->euler2;
  g1ea3 = m_Grains[selectedgrain1]->euler3;
  g2ea1 = m_Grains[selectedgrain2]->euler1;
  g2ea2 = m_Grains[selectedgrain2]->euler2;
  g2ea3 = m_Grains[selectedgrain2]->euler3;
  q1[1] = m_Grains[selectedgrain1]->avg_quat[1];
  q1[2] = m_Grains[selectedgrain1]->avg_quat[2];
  q1[3] = m_Grains[selectedgrain1]->avg_quat[3];
  q1[4] = m_Grains[selectedgrain1]->avg_quat[4];
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    g1odfbin = MisorientationCalculations::calculateHexOdfBin(q1, qref, dim1, dim2, dim3);
  }
  else if (crystruct == AIM::Reconstruction::Cubic)
  {
    g1odfbin = MisorientationCalculations::calculateCubicOdfBin(q1, qref, dim1, dim2, dim3);
  }
  q1[1] = m_Grains[selectedgrain2]->avg_quat[1];
  q1[2] = m_Grains[selectedgrain2]->avg_quat[2];
  q1[3] = m_Grains[selectedgrain2]->avg_quat[3];
  q1[4] = m_Grains[selectedgrain2]->avg_quat[4];
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    g2odfbin = MisorientationCalculations::calculateHexOdfBin(q1, qref, dim1, dim2, dim3);
  }
  else if (crystruct == AIM::Reconstruction::Cubic)
  {
    g2odfbin = MisorientationCalculations::calculateCubicOdfBin(q1, qref, dim1, dim2, dim3);
  }
  double odfchange = ((actualodf[g1odfbin] - simodf[g1odfbin]) * (actualodf[g1odfbin] - simodf[g1odfbin])) - ((actualodf[g1odfbin]
      - (simodf[g1odfbin] - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol) + (double(m_Grains[selectedgrain2]->numvoxels) * resx
          * resy * resz / totalvol))) * (actualodf[g1odfbin] - (simodf[g1odfbin] - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol)
      + (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / totalvol))));
  odfchange = odfchange + (((actualodf[g2odfbin] - simodf[g2odfbin]) * (actualodf[g2odfbin] - simodf[g2odfbin])) - ((actualodf[g2odfbin]
      - (simodf[g2odfbin] - (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / totalvol) + (double(m_Grains[selectedgrain1]->numvoxels) * resx
          * resy * resz / totalvol))) * (actualodf[g2odfbin] - (simodf[g2odfbin] - (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / totalvol)
      + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol)))));
  nlist = m_Grains[selectedgrain1]->neighborlist;
  misolist = m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfarealist = m_Grains[selectedgrain1]->neighborsurfarealist;
  double mdfchange = 0;
  initializeQ(q1, g2ea1, g2ea2, g2ea3);

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  for (size_t j = 0; j < nlist->size(); j++)
  {
    int neighbor = nlist->at(j);
    if (neighbor != selectedgrain2)
    {
      MC_LoopBody1(neighbor, j, misolist, neighborsurfarealist.get(), mdfchange);
    }
  }
  nlist = m_Grains[selectedgrain2]->neighborlist;
  misolist = m_Grains[selectedgrain2]->misorientationlist;
  neighborsurfarealist = m_Grains[selectedgrain2]->neighborsurfarealist;

  initializeQ(q1, g1ea1, g1ea2, g1ea3);

  // -----------------------------------------------------------------------------
  // mark1
  // -----------------------------------------------------------------------------
  for (size_t j = 0; j < nlist->size(); j++)
  {
    int neighbor = nlist->at(j);
    if (neighbor != selectedgrain1)
    {
      MC_LoopBody1(neighbor, j, misolist, neighborsurfarealist.get(), mdfchange);
    }
  }
  deltaerror = 1.0 * odfchange + 1.0 * mdfchange;
  if (deltaerror > 0)
  {
    badtrycount = 0;
    m_Grains[selectedgrain1]->euler1 = g2ea1;
    m_Grains[selectedgrain1]->euler2 = g2ea2;
    m_Grains[selectedgrain1]->euler3 = g2ea3;
    m_Grains[selectedgrain2]->euler1 = g1ea1;
    m_Grains[selectedgrain2]->euler2 = g1ea2;
    m_Grains[selectedgrain2]->euler3 = g1ea3;
    simodf[g1odfbin] = simodf[g1odfbin] + (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / totalvol)
        - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol);
    simodf[g2odfbin] = simodf[g2odfbin] + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol)
        - (double(m_Grains[selectedgrain2]->numvoxels) * resx * resy * resz / totalvol);
    nlist = m_Grains[selectedgrain1]->neighborlist;
    misolist = m_Grains[selectedgrain1]->misorientationlist;
    neighborsurfarealist = m_Grains[selectedgrain1]->neighborsurfarealist;

    initializeQ(q1, g2ea1, g2ea2, g2ea3);
    m_Grains[selectedgrain1]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain1]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain1]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain1]->avg_quat[4] = q1[4];
    // -----------------------------------------------------------------------------
    //  Mark2
    // -----------------------------------------------------------------------------
    for (size_t j = 0; j < nlist->size(); j++)
    {
      int neighbor = nlist->at(j);
      if (neighbor != selectedgrain2)
      {
        MC_LoopBody2(neighbor, j, misolist, neighborsurfarealist.get());
      }
    }
    nlist = m_Grains[selectedgrain2]->neighborlist;
    misolist = m_Grains[selectedgrain2]->misorientationlist;
    neighborsurfarealist = m_Grains[selectedgrain2]->neighborsurfarealist;

    initializeQ(q1, g1ea1, g1ea2, g1ea3);
    m_Grains[selectedgrain2]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain2]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain2]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain2]->avg_quat[4] = q1[4];
    // -----------------------------------------------------------------------------
    //  Mark3
    // -----------------------------------------------------------------------------
    for (size_t j = 0; j < nlist->size(); j++)
    {
      int neighbor = nlist->at(j);
      if (neighbor != selectedgrain1)
      {
        MC_LoopBody2(neighbor, j, misolist, neighborsurfarealist.get());
      }
    }
  }

}
