
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorFunc::calculateHexOdfBin( double q1[5],
                                           double qref[5],
                                           double dim1,
                                           double dim2,
                                           double dim3)
{
  double w;
  double n1;
  double n2;
  double n3;
  double degtorad = m_pi / 180.0;
  double denom;
  int g1euler1bin;
  int g1euler2bin;
  int g1euler3bin;
  int g1odfbin;

  w = MisorientationCalculations::getMisoQuatHexagonal(q1, qref, n1, n2, n3);
  w = w * degtorad;
  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = pow(denom, 0.5);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  n1 = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  n2 = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  n3 = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  g1euler1bin = int(n1 * 36.0 / dim1);
  g1euler2bin = int(n2 * 36.0 / dim2);
  g1euler3bin = int(n3 * 12.0 / dim3);
  if (g1euler1bin >= 36) g1euler1bin = 35;
  if (g1euler2bin >= 36) g1euler2bin = 35;
  if (g1euler3bin >= 12) g1euler3bin = 11;
  g1odfbin = (g1euler3bin * 36 * 36) + (g1euler2bin * 36) + (g1euler1bin);
  return g1odfbin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrainGeneratorFunc::calculateCubicOdfBin( double q1[5],
                                               double qref[5],
                                               double dim1,
                                               double dim2,
                                               double dim3)
{
  double w;
  double n1;
  double n2;
  double n3;
  double degtorad = m_pi / 180.0;
  double denom;
  int g1euler1bin;
  int g1euler2bin;
  int g1euler3bin;
  int g1odfbin;
  w = MisorientationCalculations::getMisoQuatCubic(q1, qref, n1, n2, n3);
  w = w*degtorad;
  denom = (n1*n1)+(n2*n2)+(n3*n3);
  denom = pow(denom,0.5);
  n1 = n1/denom;
  n2 = n2/denom;
  n3 = n3/denom;
  n1 = n1*pow(((3.0/4.0)*(w-sin(w))),(1.0/3.0));
  n2 = n2*pow(((3.0/4.0)*(w-sin(w))),(1.0/3.0));
  n3 = n3*pow(((3.0/4.0)*(w-sin(w))),(1.0/3.0));
  g1euler1bin = int(n1*18.0/dim1);
  g1euler2bin = int(n2*18.0/dim2);
  g1euler3bin = int(n3*18.0/dim3);
  if(g1euler1bin >= 18) g1euler1bin = 17;
  if(g1euler2bin >= 18) g1euler2bin = 17;
  if(g1euler3bin >= 18) g1euler3bin = 17;
  g1odfbin = (g1euler3bin*18*18)+(g1euler2bin*18)+(g1euler1bin);
  return g1odfbin;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrainGeneratorFunc::calculateMisorientationAngles(double &w, double &miso1, double &miso2, double &miso3)
{
  double degtorad = m_pi / 180.0;
  double denom;
  double n1, n2, n3;

  w = w * degtorad;
  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = pow(denom, 0.5);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  miso1 = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  miso2 = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  miso3 = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
}


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
  double degtorad = m_pi / 180.0;
  double denom;

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
  q2[1] = grains[neighbor].avg_quat[1];
  q2[2] = grains[neighbor].avg_quat[2];
  q2[3] = grains[neighbor].avg_quat[3];
  q2[4] = grains[neighbor].avg_quat[4];
  if(crystruct == AIM::Reconstruction::Hexagonal)
  {
    w = MisorientationCalculations::getMisoQuatHexagonal(q1,q2,n1,n2,n3);
    calculateMisorientationAngles(w, miso1, miso2, miso3);
    newmisobin = MisorientationCalculations::getMisoBinHexagonal(miso1, miso2, miso3);
  }
  else if(crystruct == AIM::Reconstruction::Cubic)
  {
    w = MisorientationCalculations::getMisoQuatCubic(q1,q2,n1,n2,n3);
    calculateMisorientationAngles(w, miso1, miso2, miso3);
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
  double degtorad = m_pi / 180.0;
  double denom;

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
  q2[1] = grains[neighbor].avg_quat[1];
  q2[2] = grains[neighbor].avg_quat[2];
  q2[3] = grains[neighbor].avg_quat[3];
  q2[4] = grains[neighbor].avg_quat[4];
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    w = MisorientationCalculations::getMisoQuatHexagonal(q1, q2, n1, n2, n3);
    calculateMisorientationAngles(w, miso1, miso2, miso3);
    newmisobin = MisorientationCalculations::getMisoBinHexagonal(miso1, miso2, miso3);
  }
  else if (crystruct == AIM::Reconstruction::Cubic)
  {
    w = MisorientationCalculations::getMisoQuatCubic(q1, q2, n1, n2, n3);
    calculateMisorientationAngles(w, miso1, miso2, miso3);
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
void GrainGeneratorFunc::initializeDims(double &dim1, double &dim2, double &dim3, int &numbins)
{
  if(crystruct == AIM::Reconstruction::Cubic)
    {
      dim1 = CubicDim1InitValue;
      dim2 = CubicDim2InitValue;
      dim3 = CubicDim3InitValue;
      numbins = 18*18*18;
    }
    if(crystruct == AIM::Reconstruction::Hexagonal)
    {
      dim1 = HexDim1InitValue;
      dim2 = HexDim2InitValue;
      dim3 = HexDim3InitValue;
      numbins = 36*36*12;
    }
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

  vector<int>* nlist;
  vector<double>* misolist;
  vector<double>* neighborsurfarealist;

  double totaldensity = 0;

  initializeDims(dim1, dim2, dim3, numbins);
  good = 0;
  while (good == 0)
  {
    selectedgrain1 = int(rg.Random() * numgrains);
    if (selectedgrain1 == 0) selectedgrain1 = 1;
    if (selectedgrain1 == numgrains) selectedgrain1 = numgrains - 1;
    if (grains[selectedgrain1].surfacegrain >= 0) good = 1;
  }
  q1[1] = grains[selectedgrain1].avg_quat[1];
  q1[2] = grains[selectedgrain1].avg_quat[2];
  q1[3] = grains[selectedgrain1].avg_quat[3];
  q1[4] = grains[selectedgrain1].avg_quat[4];
  if (crystruct == AIM::Reconstruction::Hexagonal)
  {
    g1odfbin = calculateHexOdfBin(q1, qref, dim1, dim2, dim3);
  }
  if (crystruct == AIM::Reconstruction::Cubic)
  {
    g1odfbin = calculateCubicOdfBin(q1, qref, dim1, dim2, dim3);
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
      + (double(grains[selectedgrain1].numvoxels) * resx * resy * resz / totalvol))) * (actualodf[choose] - (simodf[choose]
      + (double(grains[selectedgrain1].numvoxels) * resx * resy * resz / totalvol))));
  odfchange = odfchange + (((actualodf[g1odfbin] - simodf[g1odfbin]) * (actualodf[g1odfbin] - simodf[g1odfbin])) - ((actualodf[g1odfbin] - (simodf[g1odfbin]
      - (double(grains[selectedgrain1].numvoxels) * resx * resy * resz / totalvol))) * (actualodf[g1odfbin] - (simodf[g1odfbin]
      - (double(grains[selectedgrain1].numvoxels) * resx * resy * resz / totalvol)))));
  nlist = grains[selectedgrain1].neighborlist;
  misolist = grains[selectedgrain1].misorientationlist;
  neighborsurfarealist = grains[selectedgrain1].neighborsurfarealist;
  double mdfchange = 0;

  // -----------------------------------------------------------------------------
  // mark1
  // -----------------------------------------------------------------------------
  for (size_t j = 0; j < nlist->size(); j++)
  {
    int neighbor = nlist->at(j);
    MC_LoopBody1(neighbor, j, misolist, neighborsurfarealist, mdfchange);
  }
  deltaerror = 1.0 * odfchange + 1.0 * mdfchange;
  if (deltaerror > 0)
  {
    badtrycount = 0;
    grains[selectedgrain1].euler1 = g1ea1;
    grains[selectedgrain1].euler2 = g1ea2;
    grains[selectedgrain1].euler3 = g1ea3;
    grains[selectedgrain1].avg_quat[1] = q1[1];
    grains[selectedgrain1].avg_quat[2] = q1[2];
    grains[selectedgrain1].avg_quat[3] = q1[3];
    grains[selectedgrain1].avg_quat[4] = q1[4];
    simodf[choose] = simodf[choose] + (double(grains[selectedgrain1].numvoxels) * resx * resy * resz / totalvol);
    simodf[g1odfbin] = simodf[g1odfbin] - (double(grains[selectedgrain1].numvoxels) * resx * resy * resz / totalvol);
    // -----------------------------------------------------------------------------
    //  Mark2
    // -----------------------------------------------------------------------------
    for (size_t j = 0; j < nlist->size(); j++)
    {
      int neighbor = nlist->at(j);
      MC_LoopBody2(neighbor, j, misolist, neighborsurfarealist);
    }
  }

}
