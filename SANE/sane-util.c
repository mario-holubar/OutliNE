
/*** sane-util.c v.2.0
     David Moriarty
     April 1997

     This file contains some basic utility functions for maintaining
     SANE populations.
    
     ***/

#include "sane.h"
#include "sane-util.h"

/** reseed does just that.  It seeds the lrand48() function.
     **/

void reseed(val)
  int val;
{
  srand48(val);
  srand(val);
}

/** create_pop creates a random population of neurons
    **/

void create_pop(neuron *new_pop[])
{
  int i,j;

 if (SEED_FLAG)
  for (i=0;i<POP_SIZE;++i) {
    new_pop[i]->decoded = 0; /*reset decoded flag*/
    for(j=0;j<GENE_SIZE;++j) {
      if (j%2)
        new_pop[i]->gene[j] = randfloat()*1.0 - 0.5;
      else
        new_pop[i]->gene[j] = (float) randint(0,NUM_INPUTS+NUM_OUTPUTS-1);
    }
  }
 else 
  for (i=0;i<POP_SIZE;++i) {
    new_pop[i]->decoded = 0; /*reset decoded flag*/
    for(j=0;j<GENE_SIZE;++j) {
      if (j%2)
        new_pop[i]->gene[j] = normal_dist(0.0,2.0);
      /*randfloat()*8.0 - 4.0;*/
      else
        new_pop[i]->gene[j] = (float) randint(0,NUM_INPUTS+NUM_OUTPUTS-1);
    }
  }
}


/** load_pop loads a population from a binary file into pop.  The binary 
    **/

void load_pop(neuron *pop[],char *fname)
{
  int i,j,tmp,g_size;
  FILE *fptr;

  if ((fptr = fopen(fname,"r")) == NULL) {
     printf("\n Error - cannot open %s",fname);
     exit(1);
   }

  fread(&tmp,4,1,fptr);
  if (tmp != POP_SIZE) {
     printf("\n Error - population size does not match");
     exit(1);
   }
  fread(&g_size,4,1,fptr);

  for(i=0;i<POP_SIZE;++i) {
    pop[i]->decoded = 0;
    for(j=0;j<g_size;++j) 
       fread(&pop[i]->gene[j],sizeof(float),1,fptr);
  }
  fclose(fptr);

}
 
/** load_partial will load a saved subpopulation into the first
     NUM_HIDDEN spaces in pop.
     **/

void load_partial(neuron *pop[],char *fname)
{
  int i,j,tmp,g_size;
  FILE *fptr;

  if ((fptr = fopen(fname,"r")) == NULL) {
     printf("\n Error - cannot open %s",fname);
     exit(1);
   }

  fread(&tmp,4,1,fptr);
  if (tmp != NUM_HIDDEN) {
     printf("\n Error - population size does not match");
     exit(1);
   }
  fread(&g_size,4,1,fptr);

  for(i=0;i<NUM_HIDDEN;++i) {
    pop[i]->decoded = 0;
    for(j=0;j<g_size;++j) 
       fread(&pop[i]->gene[j],sizeof(float),1,fptr);
  }
  fclose(fptr);
}
 
/** save_pop will save a neuron population pointed to by pop into a
    binary file.
     **/

void save_pop(neuron *pop[],char *fname) 
{ 
  int i,j,tmp; 
  FILE *fptr;

  if ((fptr = fopen(fname,"wb")) == NULL) {
     printf("\n Error - cannot open %s",fname);
     exit(1);
   }

  tmp = POP_SIZE;
  fwrite(&tmp,4,1,fptr);
  tmp = GENE_SIZE;
  fwrite(&tmp,4,1,fptr);

  for(i=0;i<POP_SIZE;++i)
    for(j=0;j<GENE_SIZE;++j)
       fwrite(&pop[i]->gene[j],sizeof(float),1,fptr);

  fclose(fptr);
}

/** save_partial will save the first NUM_HIDDEN neurons in pop to a binary file.
     **/

void save_partial(neuron *pop[],char *fname)
{
  int i,j,tmp;
  FILE *fptr;

  if ((fptr = fopen(fname,"wb")) == NULL) {
     printf("\n Error - cannot open %s",fname);
     exit(1);
   }

  tmp = NUM_HIDDEN;
  fwrite(&tmp,4,1,fptr);
  tmp = GENE_SIZE;
  fwrite(&tmp,4,1,fptr);

  for(i=0;i<NUM_HIDDEN;++i)
    for(j=0;j<GENE_SIZE;++j)
       fwrite(&pop[i]->gene[j],4,1,fptr);
  fclose(fptr);
}

/** fm_output will output a population to a file compatible with
     Risto's feature map program, fm.  This is handy for visualizing
     specializations or niches. 
     **/

void fm_output(neuron *pop[],char *fname)
{
  int i,j;
  FILE *fptr;

  if ((fptr = fopen(fname,"wb")) == NULL) {
     printf("\n Error - cannot open %s in fm_output",fname);
     exit(1);
   }
  
  fprintf(fptr,"labels-and-vectors\n");

  for(i=0;i<POP_SIZE;++i) {
    fprintf(fptr,"%d ",i);
    for(j=0;j<GENE_SIZE;j+=2)   /*loop for # of connections in neuron*/    
      fprintf(fptr,"%d %f ",(int)pop[i]->gene[j],pop[i]->gene[j+1]);
    fprintf(fptr,"\n");
  }
  fclose(fptr);
}


void cluster_output(neuron *pop[],char *fname1,char *fname2,int size)
{
  int i,j,k;
  float weight[NUM_INPUTS+NUM_OUTPUTS];
  FILE *fptr1,*fptr2;

  if ((fptr1 = fopen(fname1,"wb")) == NULL) {
     printf("\n Error - cannot open %s in cluster_output_real",fname1);
     exit(1);
   }

  if ((fptr2 = fopen(fname2,"wb")) == NULL) {
     printf("\n Error - cannot open %s in cluster_output_real",fname2);
     exit(1);
   }
 

  for(i=0;i<size;++i) {
    fprintf(fptr2,"%d \n",i);
    for(j=0;j<NUM_INPUTS+NUM_OUTPUTS;++j)
      weight[j] = 0.0;
    for(j=0;j<GENE_SIZE;j+=2)  {  /*loop for # of connections in neuron*/    
      k = pop[i]->gene[j];
      if (k >= 128)
        k = k%NUM_INPUTS;
      else
        k = (k%NUM_OUTPUTS)+NUM_INPUTS;
      weight[k] += pop[i]->gene[j+1];
    }
    for(j=0;j<NUM_INPUTS+NUM_OUTPUTS;++j)
      fprintf(fptr1,"%f ",weight[j]);
    fprintf(fptr1,"\n");
  }
  fclose(fptr1);
  fclose(fptr2);
}


/** Phi returns the diversity level of the population.  The diversity
     level is determined by the average hamming distance between any two
     individuals divided by the length of the chromosome.

     Broken in sane 2.0
     **/

float Phi(neuron *pop[])
{
  int i,j;
  int h_dist=0;      /*sum of hamming distances*/
  float comparisons; /*total number of hamming comparisons*/

  /* get hamming distance between every 2 individuals */

  for(i=0;i<POP_SIZE;++i)
    for(j=i+1;j<POP_SIZE;++j)
       h_dist += hamming(pop[i]->gene,pop[j]->gene,GENE_SIZE);

  comparisons = (POP_SIZE * (POP_SIZE - 1)) / 2.0;

  return ((h_dist/comparisons)/GENE_SIZE);

  /*printf("G:%d\t%f\n",generation,(h_dist/comparisons)/GENE_SIZE);*/
}

/** hamming() computes the hamming distance between two genes.  The
     genes must be bitwise.

     Broken in sane 2.0
     **/

int hamming(float *gene1,float *gene2,int size)
{
  return 0;
}

void print_net(network *net)
{
  int i,j;

  for(i=0;i<NUM_HIDDEN;++i) {
    printf("\nHidden Unit %d\n  Inputs:",i);
    for(j=0;j<net->hidden[i]->numin;++j)
      printf(" %d,%.3f",net->hidden[i]->in_conn[j]
                           ,net->hidden[i]->in_weight[j]);
    printf("\n  Outputs:");
    for(j=0;j<net->hidden[i]->numout;++j)
      printf(" %d,%.3f",net->hidden[i]->out_conn[j]
                           ,net->hidden[i]->out_weight[j]);
  }
}

float sgn(float x)
{
  if (x > 0)
    return 1.0;
  else if (x < 0)
    return -1.0;
  else
    return 0.0;
}


double normal_dist(double avg,double sd)
{
  double fac, r, v1, v2;

  if (sd == 0.0)
    return avg;

  do {
     v1 = 2.0 * randfloat() - 1.0;
     v2 = 2.0 * randfloat() - 1.0;
     r = v1*v1 + v2*v2;
   }
  while (r >= 1.0);

  fac = sqrt(-2.0 * log(r)/r);
  return (v2*fac*sd + avg);
}  
