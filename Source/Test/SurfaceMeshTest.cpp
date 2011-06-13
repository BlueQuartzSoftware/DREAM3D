/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <iostream>
#include <string>


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265

// data structure...
struct node{
  double coord[3];
};

struct patch{
  int v_id[3];
  int nSpin[2];
};


// function prototypes...
void read_nodes (struct node *v, int nN, char *fn);
void read_triangles (struct patch *t, int nT, char *fn);
void get_inp (struct node *v, struct patch *t, int nN, int nT, char *fn);

// Main function starts...
int main(void){

  FILE *f1, *f2;

  char name1[128];
  char name2[128];
  char name3[128];

  int numN, numT;          // number of nodes and triangles...

  struct node *vertex;
  struct patch *triangle;

  printf("\nEnter the name of node input file:\n");
  scanf("%s", name1);
  printf("\nEnter the name of triangle input file:\n");
  scanf("%s", name2);
  printf("\nEnter the name of output inp file (xxx.inp):\n");
  scanf("%s", name3);

  if( (f1=fopen(name1, "r")) == NULL){
    printf("\nThe input file doesn't exist!\n");
    exit(1);
  }

  if( (f2=fopen(name2, "r")) == NULL){
    printf("\nThe input file doesn't exist!\n");
    exit(1);
  }

  fscanf(f1, "%d", &numN);
  fscanf(f2, "%d", &numT);

  fclose(f1);
  fclose(f2);

  printf("\tNumber of nodes and triangles:\n");
  printf("%10d %10d\n", numN, numT);

  vertex = (struct node *)malloc(numN*sizeof(struct node));
  triangle = (struct patch *)malloc(numT*sizeof(struct patch));

  printf("\nReading node information...\n");
  read_nodes (vertex, numN, name1);

  printf("\nReading triangle information...\n");
  read_triangles(triangle, numT, name2);

  printf("\nOutputing inp file...\n");
  get_inp (vertex, triangle, numN, numT, name3);

  return 0;
}





// Functions...
void read_nodes (struct node *v, int nN, char *fn){

  int i;

  FILE *f;

  int dummy;
  int nk;
  double x, y, z;

  if( (f=fopen(fn, "r")) == NULL){
    printf("\nThe input file doesn't exist!\n");
    exit(1);
  }

  fscanf(f, "%d", &dummy);

  for(i=0; i<nN; i++){

    fscanf(f, "%d %d %lf %lf %lf", &dummy, &nk, &x, &y, &z);
    v[i].coord[0] = x;
    v[i].coord[1] = y;
    v[i].coord[2] = z;
  }

  fclose(f);

}




void read_triangles (struct patch *t, int nT, char *fn){

  int i;

  FILE *f;

  int dummy;
  int n1, n2, n3;
  int s1, s2;
  int e1, e2, e3;

  if( (f=fopen(fn, "r")) == NULL){
    printf("\nThe input file doesn't exist!\n");
    exit(1);
  }

  fscanf(f, "%d", &dummy);

  for(i=0; i<nT; i++){

    fscanf(f, "%d %d %d %d %d %d %d %d %d",
     &dummy, &n1, &n2, &n3, &e1, &e2, &e3, &s1, &s2);

    t[i].v_id[0] = n1;
    t[i].v_id[1] = n2;
    t[i].v_id[2] = n3;

    t[i].nSpin[0] = s1;
    t[i].nSpin[1] = s2;
  }

  fclose(f);


}


void get_inp (struct node *v, struct patch *t, int nN, int nT, char *fn){

  int i, j, k;

  FILE *f;

  char type[256] = "tri";

  if( (f=fopen(fn, "w")) == NULL){
    printf("\nThe output file cannot be created!\n");
    exit(1);
  }

  fprintf(f, "%d %d %d %d %d\n", nN, nT, 0, 2, 0);

  for(i=0; i<nN; i++){
    fprintf(f, "%10d %10.5f %10.5f %10.5f\n",
      i, v[i].coord[0], v[i].coord[1], v[i].coord[2]);
  }

  for(j=0; j<nT; j++){
    fprintf(f, "%10d %2d %s %10d %10d %10d\n",
      j, 0, type, t[j].v_id[0], t[j].v_id[1], t[j].v_id[2]);
  }

  fprintf(f, "\n\n");

  fprintf(f, "2 1 1\n");
  fprintf(f, "upID, none\n");
  fprintf(f, "downID, none\n");

  for(k=0; k<nT; k++){
    fprintf(f, "%10d %5d %5d\n",
      k, t[k].nSpin[0], t[k].nSpin[1]);
  }

}




