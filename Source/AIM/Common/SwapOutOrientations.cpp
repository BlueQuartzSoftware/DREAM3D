


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorFunc::MC_LoopBody1(size_t neighbor, int j,
                                      vector<double>* misolist,
                                      vector<double>* neighborsurfarealist,
                                      double &mdfchange)
{
  double w;
  double n1;
  double n2;
  double n3;
//  double degtorad = m_pi / 180.0;
//  double denom;

  int curmiso1 = std::numeric_limits<int >::max();
  int curmiso2 = std::numeric_limits<int >::max();
  int curmiso3 = std::numeric_limits<int >::max();
  int neighsurfarea = std::numeric_limits<int >::max();
  int curmisobin = std::numeric_limits<int >::max();
  int newmisobin = std::numeric_limits<int >::max();
  double q1[5], q2[5];
  double miso1 = std::numeric_limits<double >::max();
  double miso2 = std::numeric_limits<double >::max();
  double miso3 = std::numeric_limits<double >::max();


  curmiso1 = misolist->at(3*j);
  curmiso2 = misolist->at(3*j+1);
  curmiso3 = misolist->at(3*j+2);
  neighsurfarea = neighborsurfarealist->at(j);
  if(crystruct == AIM::Reconstruction::Cubic)
  {
    curmisobin = MisorientationCalculations::getMisoBinCubic(curmiso1, curmiso2, curmiso3);
  }
  if(crystruct == AIM::Reconstruction::Hexagonal)
  {
    curmisobin = MisorientationCalculations::getMisoBinHexagonal(curmiso1, curmiso2, curmiso3);
  }
  q2[1] = m_Grains[neighbor]->avg_quat[1];
  q2[2] = m_Grains[neighbor]->avg_quat[2];
  q2[3] = m_Grains[neighbor]->avg_quat[3];
  q2[4] = m_Grains[neighbor]->avg_quat[4];
  if(crystruct == AIM::Reconstruction::Hexagonal)
  {
    w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
    MisorientationCalculations::calculateMisorientationAngles(w, miso1, miso2, miso3);
    newmisobin = MisorientationCalculations::getMisoBinHexagonal(miso1, miso2, miso3);
  }
  else if(crystruct == AIM::Reconstruction::Cubic)
  {
    w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
    MisorientationCalculations::calculateMisorientationAngles(w, miso1, miso2, miso3);
    newmisobin = MisorientationCalculations::getMisoBinCubic(miso1, miso2, miso3);
  }
  mdfchange = mdfchange + (((actualmdf[curmisobin]-simmdf[curmisobin])*(actualmdf[curmisobin]-simmdf[curmisobin])) - ((actualmdf[curmisobin]-(simmdf[curmisobin]-(neighsurfarea/totalsurfacearea)))*(actualmdf[curmisobin]-(simmdf[curmisobin]-(neighsurfarea/totalsurfacearea)))));
  mdfchange = mdfchange + (((actualmdf[newmisobin]-simmdf[newmisobin])*(actualmdf[newmisobin]-simmdf[newmisobin])) - ((actualmdf[newmisobin]-(simmdf[newmisobin]+(neighsurfarea/totalsurfacearea)))*(actualmdf[newmisobin]-(simmdf[newmisobin]+(neighsurfarea/totalsurfacearea)))));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorFunc::MC_LoopBody2(size_t neighbor, int j,
                                      vector<double>* misolist,
                                      vector<double>* neighborsurfarealist)
{
  double w;
  double n1;
  double n2;
  double n3;
//  double degtorad = m_pi / 180.0;
//  double denom;

  int curmiso1 = std::numeric_limits<int >::max();
  int curmiso2 = std::numeric_limits<int >::max();
  int curmiso3 = std::numeric_limits<int >::max();
  int neighsurfarea = std::numeric_limits<int >::max();
  int curmisobin = std::numeric_limits<int >::max();
  int newmisobin = std::numeric_limits<int >::max();
  double q1[5], q2[5];
  double miso1 = std::numeric_limits<double >::max();
  double miso2 = std::numeric_limits<double >::max();
  double miso3 = std::numeric_limits<double >::max();

  curmiso1 = misolist->at(3 * j);
  curmiso2 = misolist->at(3 * j + 1);
  curmiso3 = misolist->at(3 * j + 2);
  neighsurfarea = neighborsurfarealist->at(j);
  if (crystruct == AIM::Reconstruction::Cubic)
  {
    curmisobin = MisorientationCalculations::getMisoBinCubic(curmiso1, curmiso2, curmiso3);
  }
  else if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    curmisobin = MisorientationCalculations::getMisoBinHexagonal(curmiso1, curmiso2, curmiso3);
  }
  q2[1] = m_Grains[neighbor]->avg_quat[1];
  q2[2] = m_Grains[neighbor]->avg_quat[2];
  q2[3] = m_Grains[neighbor]->avg_quat[3];
  q2[4] = m_Grains[neighbor]->avg_quat[4];
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
    MisorientationCalculations::calculateMisorientationAngles(w, miso1, miso2, miso3);
    newmisobin = MisorientationCalculations::getMisoBinHexagonal(miso1, miso2, miso3);
  }
  else if (crystruct == AIM::Reconstruction::Cubic)
  {
    w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
    MisorientationCalculations::calculateMisorientationAngles(w, miso1, miso2, miso3);
    newmisobin = MisorientationCalculations::getMisoBinCubic(miso1, miso2, miso3);
  }
  misolist->at(3 * j) = miso1;
  misolist->at(3 * j + 1) = miso2;
  misolist->at(3 * j + 2) = miso3;
  simmdf[curmisobin] = simmdf[curmisobin] - (neighsurfarea / totalsurfacearea);
  simmdf[newmisobin] = simmdf[newmisobin] + (neighsurfarea / totalsurfacearea);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorFunc::initializeQ(double* q, double e1, double e2, double e3)
{
  double s, c, s1, c1, s2, c2;
  s = sin(0.5 * e2);
  c = cos(0.5 * e2);
  s1 = sin(0.5 * (e1 - e3));
  c1 = cos(0.5 * (e1 - e3));
  s2 = sin(0.5 * (e1 + e3));
  c2 = cos(0.5 * (e1 + e3));
  q[1] = s*c1;
  q[2] = s*s1;
  q[3] = c*s2;
  q[4] = c*c2;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorFunc::swapOutOrientation( int &badtrycount, int &numbins)
{
  double random;
  int good;
  double deltaerror = 1.0;
  int selectedgrain1;
  double q1[5];
  double qref[5];
  qref[1] = SinOfZero * CosOfZero; // sin(0.5 * 0.0) * cos(0.5 * (0.0 - 0.0));
  qref[2] = SinOfZero * SinOfZero; // sin(0.5 * 0.0) * sin(0.5 * (0.0 - 0.0));
  qref[3] = CosOfZero * SinOfZero; // cos(0.5 * 0.0) * sin(0.5 * (0.0 + 0.0));
  qref[4] = CosOfZero * CosOfZero; // cos(0.5 * 0.0) * cos(0.5 * (0.0 + 0.0));

  double dim1 = std::numeric_limits<double >::max(), dim2 = std::numeric_limits<double >::max(), dim3 = std::numeric_limits<double >::max();

  int g1odfbin = std::numeric_limits<int >::max();

  int phi1 = std::numeric_limits<int >::max(), PHI = std::numeric_limits<int >::max(), phi2 = std::numeric_limits<int >::max();
  double chooseh1 = std::numeric_limits<double >::max(), chooseh2 = std::numeric_limits<double >::max(), chooseh3 = std::numeric_limits<double >::max();
  double chooser1 = std::numeric_limits<double >::max(), chooser2 = std::numeric_limits<double >::max(), chooser3 = std::numeric_limits<double >::max();
  double g1ea1 = std::numeric_limits<double >::max(), g1ea2 = std::numeric_limits<double >::max(), g1ea3 = std::numeric_limits<double >::max();

  double hmag = std::numeric_limits<double >::max(), angle = std::numeric_limits<double >::max();

  double sum = std::numeric_limits<double >::max(), diff = std::numeric_limits<double >::max();

  IntVectorType nlist;
  vector<double>* misolist;
  DoubleVectorType neighborsurfarealist;

  double totaldensity = 0;

  MisorientationCalculations::initializeDims(crystruct, dim1, dim2, dim3, numbins);
  good = 0;
  while (good == 0)
  {
    selectedgrain1 = int(rg.Random() * numgrains);
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == numgrains) selectedgrain1 = numgrains - 1;
    if (m_Grains[selectedgrain1]->surfacegrain >= 0) good = 1;
  }
  q1[1] = m_Grains[selectedgrain1]->avg_quat[1];
  q1[2] = m_Grains[selectedgrain1]->avg_quat[2];
  q1[3] = m_Grains[selectedgrain1]->avg_quat[3];
  q1[4] = m_Grains[selectedgrain1]->avg_quat[4];
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    g1odfbin = MisorientationCalculations::calculateHexOdfBin(q1, qref, dim1, dim2, dim3);
  }
  if (crystruct == AIM::Reconstruction::Cubic)
  {
    g1odfbin = MisorientationCalculations::calculateCubicOdfBin(q1, qref, dim1, dim2, dim3);
  }
  random = rg.Random();
  int choose = 0;
  totaldensity = 0;
  for (int i = 0; i < numbins; i++)
  {
    double density = actualodf[i];
    totaldensity = totaldensity + density;
    if (random >= totaldensity) choose = i;
  }
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    phi1 = choose % 36;
    PHI = (choose / 36) % 36;
    phi2 = choose / (36 * 36);
  }
  if (crystruct == AIM::Reconstruction::Cubic)
  {
    phi1 = choose % 18;
    PHI = (choose / 18) % 18;
    phi2 = choose / (18 * 18);
  }
  random = rg.Random();
  chooseh1 = (dim1 * phi1) + (dim1 * random);
  random = rg.Random();
  chooseh2 = (dim2 * PHI) + (dim2 * random);
  random = rg.Random();
  chooseh3 = (dim3 * phi2) + (dim3 * random);
  hmag = pow((chooseh1 * chooseh1 + chooseh2 * chooseh2 + chooseh3 * chooseh3), 0.5);
  angle = pow((8.25 * hmag * hmag * hmag), (1.0 / 3.0));
  chooser1 = tan(angle / 2.0) * (chooseh1 / hmag);
  chooser2 = tan(angle / 2.0) * (chooseh2 / hmag);
  chooser3 = tan(angle / 2.0) * (chooseh3 / hmag);
  sum = atan(chooser3);
  diff = atan(chooser2 / chooser1);
  g1ea1 = sum + diff;
  g1ea2 = 2. * atan(chooser1 * cos(sum) / cos(diff));
  g1ea3 = sum - diff;

  initializeQ(q1, g1ea1, g1ea2, g1ea3);
  double odfchange = ((actualodf[choose] - simodf[choose]) * (actualodf[choose] - simodf[choose])) - ((actualodf[choose] - (simodf[choose]
      + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol))) * (actualodf[choose] - (simodf[choose]
      + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol))));
  odfchange = odfchange + (((actualodf[g1odfbin] - simodf[g1odfbin]) * (actualodf[g1odfbin] - simodf[g1odfbin])) - ((actualodf[g1odfbin] - (simodf[g1odfbin]
      - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol))) * (actualodf[g1odfbin] - (simodf[g1odfbin]
      - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol)))));
  nlist = m_Grains[selectedgrain1]->neighborlist;
  misolist = m_Grains[selectedgrain1]->misorientationlist;
  neighborsurfarealist = m_Grains[selectedgrain1]->neighborsurfarealist;
  double mdfchange = 0;

  // -----------------------------------------------------------------------------
  // mark1
  // -----------------------------------------------------------------------------
  for (size_t j = 0; j < nlist->size(); j++)
  {
    int neighbor = nlist->at(j);
    MC_LoopBody1(neighbor, j, misolist, neighborsurfarealist.get(), mdfchange);
  }
  deltaerror = 1.0 * odfchange + 1.0 * mdfchange;
  if (deltaerror > 0)
  {
    badtrycount = 0;
    m_Grains[selectedgrain1]->euler1 = g1ea1;
    m_Grains[selectedgrain1]->euler2 = g1ea2;
    m_Grains[selectedgrain1]->euler3 = g1ea3;
    m_Grains[selectedgrain1]->avg_quat[1] = q1[1];
    m_Grains[selectedgrain1]->avg_quat[2] = q1[2];
    m_Grains[selectedgrain1]->avg_quat[3] = q1[3];
    m_Grains[selectedgrain1]->avg_quat[4] = q1[4];
    simodf[choose] = simodf[choose] + (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol);
    simodf[g1odfbin] = simodf[g1odfbin] - (double(m_Grains[selectedgrain1]->numvoxels) * resx * resy * resz / totalvol);
    // -----------------------------------------------------------------------------
    //  Mark2
    // -----------------------------------------------------------------------------
    for (size_t j = 0; j < nlist->size(); j++)
    {
      int neighbor = nlist->at(j);
      MC_LoopBody2(neighbor, j, misolist, neighborsurfarealist.get());
    }
  }

}
