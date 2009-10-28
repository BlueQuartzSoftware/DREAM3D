// smooth.cpp
// Written by Jason Gruber jgruber@andrew.cmu.edu

//  g++ -g -O0 -o smooth smooth-grain3d.cpp

//  look for mobility values etc, and perhaps tweak them


#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include<string>
#include<sstream>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<vector>
#include"LinearAlgebra.h"
#include<cstdlib>
#include<cstdio>
#include <fstream>
#include <ostream>
// using namespace::std ;

void tokenize(const std::string& str,
			  std::vector<std::string>& tokens,
			  const std::string& delimiters = " ")
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
    {
	  // Found a token, add it to the vector.
	  tokens.push_back(str.substr(lastPos, pos - lastPos));

	  // Skip delimiters.  Note the "not_of"
	  lastPos = str.find_first_not_of(delimiters, pos);

	  // Find next "non-delimiter"
	  pos = str.find_first_of(delimiters, lastPos);
    }
}

inline int delta(int i, int j) {return (i==j);}

class node{
  friend int& id(node&);
public:
  double& operator[](int i) {return x[i];}
  double operator[](int i) const {return x[i];}
  //  int line;
private:
  double x[3];
  int id;
  int type;
};

inline int& id(node& n) {return n.id;}

double distance(const node& n0, const node& n1)
{
  double a = n0[0]-n1[0];
  double b = n0[1]-n1[1];
  double c = n0[2]-n1[2];
  return sqrt(a*a+b*b+c*c);
}

class triangle{
public:
  node& operator[](int i) {return *p[i];}
  const node& operator[](int i) const {return *p[i];}
  node*& operator()(int i) {return p[i];}
  node* const & operator()(int i) const {return p[i];}
  double area() const;
  double aspect() const;
  double circularity(double area) const;
  Vector<double> normal() const;
  int region1;
  int region2;

private:
  node* p[3];
};

double triangle::area() const
{
  double a[3], b[3], c[3];
  a[0] = (*p[1])[0]-(*p[0])[0];
  a[1] = (*p[1])[1]-(*p[0])[1];
  a[2] = (*p[1])[2]-(*p[0])[2];
  b[0] = (*p[2])[0]-(*p[0])[0];
  b[1] = (*p[2])[1]-(*p[0])[1];
  b[2] = (*p[2])[2]-(*p[0])[2];
  c[0] = a[1]*b[2]-a[2]*b[1];
  c[1] = a[2]*b[0]-a[0]*b[2];
  c[2] = a[0]*b[1]-a[1]*b[0];
  return 0.5*sqrt(c[0]*c[0]+c[1]*c[1]+c[2]*c[2]);
}

Vector<double> triangle::normal() const
{
  Vector<double> n(3);
  double a[3], b[3];
  a[0] = (*p[1])[0]-(*p[0])[0];
  a[1] = (*p[1])[1]-(*p[0])[1];
  a[2] = (*p[1])[2]-(*p[0])[2];
  b[0] = (*p[2])[0]-(*p[0])[0];
  b[1] = (*p[2])[1]-(*p[0])[1];
  b[2] = (*p[2])[2]-(*p[0])[2];
  n[0] = a[1]*b[2]-a[2]*b[1];
  n[1] = a[2]*b[0]-a[0]*b[2];
  n[2] = a[0]*b[1]-a[1]*b[0];
  double norm = sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
  if (norm>0.0) {
	double rnorm = 1.0/norm;
	n[0] *= rnorm;
	n[1] *= rnorm;
	n[2] *= rnorm;
  }
  return n;
}

double triangle::aspect() const
{
  double a = distance(*p[0],*p[1]);
  double b = distance(*p[1],*p[2]);
  double c = distance(*p[2],*p[0]);
  double min = a;
  double max = a;
  if (b>max) max = b;
  else if (b<min) min = b;
  if (c>max) max = c;
  else if (c<min) min = c;
  return max/min;
}

double triangle::circularity(double area) const
{
  double a = distance(*p[0],*p[1]);
  double b = distance(*p[1],*p[2]);
  double c = distance(*p[2],*p[0]);
  double s = 0.5*(a+b+c);
  double r = area/s;
  double R = a*b*c/4/area;
  return R/r;
}

#ifdef SURFACE_MESH_LIBRARY
int SmoothGrain3D(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
  if( argc < 5 ) {
	std::cout << "Usage: ./smooth nodes.txt triangles.txt "<< std::endl ;
	std::cout << "  output.inp number_iters print_interval lock-quad-pts[yes=1]" << std::endl ;
	// 	std::cout << "- if you are resmoothing an already smoothed structure" << std::endl;
	// 	std::cout << "  then you can specify the starting iteration for the output file"<< std::endl;
	// 	std::cout << std::endl;
	// 	std::cout << "./smooth  output 34:200" << std::endl;
	// 	std::cout << std::endl;
	// 	std::cout << " will start the output file iterations at 34 and end at 200." << std::endl;
	exit(1);
  }

  // Data variables
  int nnod, ntri, junk ;
  std::vector<std::string> data;

  int interval = 100 ;  // default value
  if( argc >= 6 ) interval = atoi( argv[5] ) ;
  std::cout << "Interval for printout: " << interval << std::endl ;

  int lockquads = 0 ;  // default value
  if( argc >= 7 ) lockquads = atoi( argv[6] ) ;
  if ( lockquads == 1 ) {
    std::cout << "Will not allow quad points to move " << std::endl ;
  } else {
    std::cout << "Quad points are free to move " << std::endl ;
  }

  // read in nodes/triangles
  std::ifstream input1(argv[1]);
  std::ifstream input2(argv[2]);
  std::ofstream velocities;

  input1>>nnod;
  input2>>ntri;
  std::cout<< "Number of nodes: " << nnod << std::endl;
  std::cout<< "Number of triangles: " << ntri << std::endl;
  std::cout<<std::endl;

  //Allocate the triangle and node vectors
  std::vector<node> nodes(nnod);
  std::vector<triangle> triangles(ntri);
  std::vector<int> tid(ntri);
  std::vector<int> nodetype(nnod) ; //  added vi 09 to keep track of node type

  //Read the nodes
  std::cout << "reading nodes " << std::endl;
  for (int i = 0 ; i < nnod ; i++ )
    input1 >> id(nodes[i]) >> nodetype[i] >>nodes[i][0]>>nodes[i][1]>>nodes[i][2];
  //  added a junk entry, 25 vi 09; changed to nodetype, 26 vi 09
  std::cout << "end reading nodes"<< std::endl;

  //Read the triangles
  std::cout << "reading triangles: " << std::endl;
  for (int i=0; i<ntri; i++) {
	//char type[3];
	int nid[3];
	//int reg1, reg2;
	input2 >> tid[i] >> nid[0] >> nid[1] >> nid[2]
	      >> junk >> junk >> junk
	       >> triangles[i].region1 >> triangles[i].region2 ;

	triangles[i](0) = &nodes[nid[0]];
	triangles[i](1) = &nodes[nid[1]];
	triangles[i](2) = &nodes[nid[2]];
	// 	bool ok[3] = {false,false,false};
	// 	for (int j=0; j<nnod; j++) {
	// // 	  if (id[j]==nid[0]) {triangles[i](0) = &nodes[j]; triangles[i].nids[0]= nid[0]; ok[0] = true;}
	// // 	  else if (id[j]==nid[1]) {triangles[i](1) = &nodes[j]; triangles[i].nids[1]= nid[1];ok[1] = true;}
	// // 	  else if (id[j]==nid[2]) {triangles[i](2) = &nodes[j]; triangles[i].nids[2]= nid[2];ok[2] = true;}
	// 	  if (id(nodes[j])==nid[0]) {triangles[i](0) = &nodes[j];  ok[0] = true;}
	// 	  else if (id(nodes[j])==nid[1]) {triangles[i](1) = &nodes[j]; ok[1] = true;}
	// 	  else if (id(nodes[j])==nid[2]) {triangles[i](2) = &nodes[j]; ok[2] = true;}
	// 	  if (ok[0] && ok[1] && ok[2]) break;
	// 	}
  }
  std::cout << "end reading triangles" << std::endl;


  // Find the minimum and maximum dimension of the data
  double min[3] = {2.0,2.0,2.0};
  double max[3] = {0.0,0.0,0.0};

  for (size_t i = 0; i < nodes.size(); i++)
  {
    for (size_t j = 0; j < 3; j++)
    {
      if (nodes[i][j] < min[j])
        min[j] = nodes[i][j];
      if (nodes[i][j] > max[j])
        max[j] = nodes[i][j];
    }
  }
  std::cout << "Model Dimensions: " << std::endl;
  for(int i=0; i<3; i++)
	std::cout << min[i] << " " << max[i] << std::endl;

  //Allocate vectors and matricies
  int n_size = 3*nodes.size();
  Vector<double> x(n_size), F(n_size);
  SMatrix<double> K(n_size,n_size);

  //Allocate contants for solving linear equations
  const double epsilon = 1.0; // change this if quality force too
							  // high, low
  const double dt = (40.0e-6)*(10/max[1]); // time step, change if
  // mesh moves too much,
  // little
  double bc_dt ;  // for updating
  const double small = 1.0e-12;
  const double large = 1.0e+50;
  const double one12th = 1.0/12.0;
  const double tolerance = 1.0e-5; // Tolerance for nodes that are
								   // near the RVE boundary

  double A_scale, Q_scale; // Prefactors for quality and curvature forces
  //   don't make these two values too far different
  //  larger values should increase velocities


  //Variables for logging of quality progress
  double Q_max, Q_sum, Q_ave, Q_max_ave;
  int hist_count = 10;

  std::vector<double> Q_max_hist(hist_count);

  Q_ave = 2.9 ;
  Q_max_ave = 10 ;

  A_scale = 4000.0 ;
  Q_scale = 500.0 ;


  // update loop
  size_t updatesMax = atoi(argv[4]);
  for (size_t updates = 1; updates <= updatesMax; ++updates)
  {
    // changed arg index
    std::cout << "Update loop: " << updates << std::endl;

    // compute triangle contributions to K and F
    int ntri = triangles.size();
    Q_max = 0;
    Q_sum = 0;
    for (int t = 0; t < ntri; t++)
    { // Loop through number of trianges
      triangle& rtri = triangles[t];
      Vector<double> n(3);
      n = rtri.normal();
      double A = rtri.area();
      double Q = rtri.circularity(A);
      Q_sum += Q;
      if (Q > Q_max)
        Q_max = Q;
      for (int n0 = 0; n0 < 3; n0++)
      {
        int i = id(rtri[n0]);
        for (int j = 0; j < 3; j++)
        {
          nodes[i][j] += small;
          double Anew = rtri.area();
          double Qnew = rtri.circularity(Anew);
          nodes[i][j] -= small;
          // if( (Q_ave < 3.0) && (Q_max_ave < 10) ){
          //		  A_scale = 4000.0;
          //	  Q_scale = 1000.0;
          // } else {
          // 	  A_scale = 1000;
          // 	  Q_scale = 500;
          // }
          //F[3*i+j] -= ((Anew-A)*6500+(Qnew-Q)*800)/small;
          //F[3*i+j] -= ((Anew-A)*1000+(Qnew-Q)*2500*A)/small;
          F[3 * i + j] -= (A_scale * (Anew - A) + Q_scale * (Qnew - Q) * A) / small;
        }
        for (int n1 = 0; n1 < 3; n1++)
        {
          int h = id(rtri[n1]);
          for (int k = 0; k < 3; k++)
            for (int j = 0; j < 3; j++)
              K[3 * h + k][3 * i + j] += one12th * (1.0 + delta(i, h)) * n[j] * n[k] * A;
        }
      }
    }

    // add epsilon to the diagonal
    for (size_t r = 0; r < nodes.size(); r++)
      for (int s = 0; s < 3; s++)
        K[3 * r + s][3 * r + s] += epsilon;

    // apply boundary conditions
    // if node i, component j is constrained, do this...
    // K[3*i+j][3*i+j] += large;
    for (size_t r = 0; r < nodes.size(); r++)
      for (int s = 0; s < 3; s++)
      {
        if (fabs(nodes[r][s] - max[s]) < tolerance)
          K[3 * r + s][3 * r + s] += large;
        if (fabs(nodes[r][s] - min[s]) < tolerance)
          K[3 * r + s][3 * r + s] += large;
      }

    // solve for node velocities
    int iterations = CR(K, x, F, 4000, 1.0e-5);
    std::cout << iterations << " iterations ... " << std::endl;

    //Update the quality information
    if (updates - 1 < Q_max_hist.size())
    {
      Q_max_hist[updates - 1] = Q_max;
    }
    else
    {

      //Update the history of Q_max
      for (size_t i = 0; i < Q_max_hist.size() - 1; i++)
      {
        // std::cout << i << " "<< Q_max_hist[i] << " " << Q_max_hist[i+1] << std::endl;
        Q_max_hist[i] = Q_max_hist[i + 1];
      }

      Q_max_hist[Q_max_hist.size() - 1] = Q_max;

      //Compute the rolling average of the Q_max
      Q_max_ave = 0;
      for (size_t i = 0; i < Q_max_hist.size(); i++)
        Q_max_ave += Q_max_hist[i];
      Q_max_ave /= Q_max_hist.size();
    }
    Q_ave = Q_sum / ntri;

    // 	for(int i=0; i<Q_max_hist.size(); i++)
    // 	  std::cout<<"Q_Max history ... "<<Q_max_hist[i]<<std::endl;


    std::cout << "Maximum quality ... " << Q_max << std::endl;
    std::cout << "Ave_Max quality ... " << Q_max_ave << std::endl;
    std::cout << "Average quality ... " << Q_ave << std::endl;

    std::cout << std::endl;

    //Output velocities for examination
    std::ostringstream iter_stream;
    std::string iter_string;

    // write the iteration to a string
    iter_stream << updates;
    iter_string = iter_stream.str();

    // extract the basename from the provided filename
    std::string infile = argv[3];
    std::vector<std::string> tokens;
    std::string delimeters = "."; // Only a period
    std::string basename;
    tokenize(infile, tokens, delimeters);

    if (tokens.size() > 2)
    {
      std::cout << "WARNING: multiple \".\" in file name: " << infile << std::endl;
      std::cout << "\t Using: ." << tokens.front() << "As basename" << std::endl;
      basename = tokens.front();
    }
    else if (tokens.size() == 2)
    {
      basename = tokens.front();
    }

    // put it all back together
    std::string iterFileName;
    if (updates < 9)
    {
      iterFileName = basename + "_0" + iter_string + ".inp";
    }
    else
    {
      iterFileName = basename + "_" + iter_string + ".inp";
    }

    // update node positions
    for (size_t r = 0; r < nodes.size(); r++)
    {
      //	  velocityfile << r << " ";
      for (int s = 0; s < 3; s++)
      {
        bc_dt = dt;
        if ((fabs(nodes[r][s] - max[s]) < tolerance) || (fabs(nodes[r][s] - min[s]) < tolerance))
          bc_dt = 0.0;

        if (fabs(dt * x[3 * r + s]) > 1.0)
          nodes[r][s] += 0.0;
        else if (fabs(dt * x[3 * r + s]) < 1.0)
          nodes[r][s] += bc_dt * x[3 * r + s];
        // velocityfile  << std::scientific << std::setw(4)
        // << std::setprecision(4) << F[3*r+s] << "\t"<< x[3*r+s] <<"\t";
      }
      //  velocityfile << std::endl;
    }
    //	velocityfile.close();

    //	if(!((updates)%10)){
    if (!((updates) % interval))
    {
      // Open the outputfile
      std::ofstream inpfile;
      inpfile.open(iterFileName.c_str());
      if (!inpfile)
      {
        std::cout << "Failed to open: " << iterFileName << std::endl;
        exit(1);
      }

      inpfile << nnod << " " << ntri << " 0 2 0" << std::endl;
      // the "0 2 0" signals the presence of upID downID output
      for (int ia = 0; ia < nnod; ia++)
        inpfile << id(nodes[ia]) << " " << nodes[ia][0] << " " << nodes[ia][1] << " " << nodes[ia][2] << std::endl;

      for (int i = 0; i < ntri; i++)
      {
        //		inpfile<<i<<" "<<triangles[i].region1>>triangles[i].region2<<" tri ";
        inpfile << i << " 0 tri ";
        inpfile << id(triangles[i][0]) << " ";
        inpfile << id(triangles[i][1]) << " ";
        inpfile << id(triangles[i][2]) << std::endl;
        //inpfile<<triangles[i].region1<<" ";
        //inpfile<<triangles[i].region2<<std::endl;
      }

      //	  for(int i=0; i<data.size(); i++)

      inpfile << "2 1 1" << std::endl;
      inpfile << "minID, none" << std::endl;
      inpfile << "maxID, none" << std::endl;
      for (int i = 0; i < ntri; i++) // ADR  25 jun 09
        inpfile << i << " " << triangles[i].region1 << " " << triangles[i].region2 << std::endl;

      // 	  for (int i=0; i<ntri; i++) {
      // 		int r1 = triangles[i].region1;
      // 		int r2 = triangles[i].region2;
      // 		int min = (r1<r2 ? r1:r2);
      // 		int max = (r1>r2 ? r1:r2);
      // 		inpfile<<i<<" "<<min<<" "<<max<std::endl;
      // 	  }

      inpfile.close();

      inpfile.open("nodes_smoothed.txt"); // output Sukbin style nodes
      if (!inpfile)
      {
        std::cout << "Failed to open: " << "nodes_smoothed.txt" << std::endl;
        exit(1);
      }
      inpfile << nnod << std::endl;
      for (int ia = 0; ia < nnod; ia++)
        inpfile << id(nodes[ia]) << " " << nodetype[ia] << " " << nodes[ia][0] << " " << nodes[ia][1] << " " << nodes[ia][2] << std::endl;

    }

  }

  //   // output sukbin nodes file
  //   std::ofstream output(argv[2]);
  //   output<<nnod<<std::endl;
  //   for (int i=0; i<nnod; i++){
  // 	output<<std::setw(6)<<std::fixed<<"\t"
  // 	  //<<id[i]<<"\t"
  // 	  //<<nodes[i].line<<"\t"
  // 		  <<nodes[i][0]<<"\t"<<nodes[i][1]<<"\t"<<nodes[i][2]<<std::endl;
  //   }


  //   std::ofstream avs("output.inp");
  //   avs<<nnod<<" "<<ntri<<" 0 0 0"<<std::endl;
  //   for (int i=0; i<nnod; i++)
  // 	avs<<id(nodes[i])<<" "<<nodes[i][0]<<" "<<nodes[i][1]<<" "<<nodes[i][2]<<std::endl;

  //   for (int i=0; i<ntri; i++) {
  // 	avs<<i<<" 0 tri ";
  // 	avs<<id(triangles[i][0])<<" ";
  // 	avs<<id(triangles[i][1])<<" ";
  // 	avs<<id(triangles[i][2])<<std::endl;
  //   }

  //   std::ofstream testput("test.txt");
  //   testput<<ntri<<std::endl;
  //   for (int i=0; i<ntri; i++){
  // 	testput<<tid[i]<< std::endl;
  // 	for(int j=0; j<3; j++)
  // 	  testput << triangles[i].nids[j] << "\t" << std::endl;
  // 	testput << std::endl;

  //   }

  // 	for (int i=0; i<ntri; i++) {
  // 		output<<tid[i]<<" tri ";
  // 		output<<id[(&triangles[i][0]-&nodes[0])/sizeof(node)]<<" ";
  // 		output<<id[(&triangles[i][1]-&nodes[0])/sizeof(node)]<<" ";
  // 		output<<id[(&triangles[i][2]-&nodes[0])/sizeof(node)]<<std::endl;
  // 	}
}

