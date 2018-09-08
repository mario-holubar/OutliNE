/*** sane-seed.c 
     David Moriarty
     April 1997

sane-seed.c contains the functions for seeding a population with prexisting
domain knowledge.  The domain knowledge is specified in the file
seed.txt, which contains a series of input/output pairs.  The i/o
pairs represent some examples off decisions using some rule of thumb
strategies or expert opinions.

***/

#include "sane.h"
#include "sane-seed.h"
#include "sane-nn.h"
#include "sane-util.h"

float seed_in[NUM_SEED_EXAMPLES][NUM_INPUTS];
float seed_out[NUM_SEED_EXAMPLES][NUM_OUTPUTS];

#define S_ETA   ETA
#define S_ALPHA ALPHA

/** seed_pop is the main seeding function.  It cycles through the
    number of networks to seed (declared by SEED_NETS), seeds them,
    and then writes the new weights back to the neuron genes.**/

void seed_pop(pop)
  neuron *pop[];
{
  int i,j;
  network net;    

  load_seed_data("seed.txt");

  for(i=0;i<SEED_NETS;++i) {
    build_net(&net,best_nets[i]->neuron);
    seed_cycle(&net,SEED_TRIALS);
    for(j=0;j<NUM_HIDDEN;++j)
      weights_to_gene(best_nets[i]->neuron[j]);
  }
  
}

/** seed_cycle uses straight backprop with learning rate S_ETA and a
    momentum term S_ALPHA to modify the weights based on performance
    over the training examples.**/

void seed_cycle(net,stop)
  network *net;
  int stop;
{
  int i,j,k,l;
  double err[NUM_OUTPUTS],h_err,delta;
  double e,mse=0.0;
  neuron *h;

  for(i=0;i<stop;++i) {
    printf("Cycle %d, MSE = %f\n",i,mse/600.0);
    mse = 0.0;
    for(j=0;j<NUM_SEED_EXAMPLES;++j) {
      for(k=0;k<NUM_INPUTS;++k) 
        net->input[k] = seed_in[j][k];
      activate_net(net); 
      /* get output errors*/
      for (k=0;k<NUM_OUTPUTS;++k)
        err[k] = (seed_out[j][k] - net->sigout[k]) 
                 * net->sigout[k] * (1-net->sigout[k]);

      for(k=0;k<NUM_OUTPUTS;++k) {
        e = seed_out[j][k] - net->sigout[k];
        mse += e*e;
      }

      /*error propogation*/
      for(k=0;k<NUM_HIDDEN;++k) {
        h = net->hidden[k];
        h_err = 0.0;
        for(l=0;l<h->numout;++l) {
          h_err += err[h->out_conn[l]] * h->out_weight[l];
          delta = S_ETA * err[h->out_conn[l]] * h->sigout 
                  + S_ALPHA * h->out_delta[l];
          h->out_weight[l] += delta;
          h->out_delta[l] = delta;
	}

       if (h_err != 0.0) {
         h_err = h_err * h->sigout * (1.0 - h->sigout);
         for(l=0;l<h->numin;++l) {
           delta = S_ETA * h_err * sgn(net->input[h->in_conn[l]])
                   + S_ALPHA * h->in_delta[l];
           h->in_weight[l] += delta;
           h->in_delta[l] = delta;
	 }
       }
      }
    }
  }
}
  
/** load_seed_data loads the i/o pairs from the file seed.txt.**/

void load_seed_data(fname)
  char *fname;
{

  FILE *fptr;
  int i,j;



  if ((fptr = fopen(fname,"r")) == NULL) {
     printf("\n Error - cannot open %s",fname);
     exit(1);
   }

  for(i=0;i<NUM_SEED_EXAMPLES;++i) {
    for(j=0;j<NUM_INPUTS;++j) 
      fscanf(fptr,"%f",&seed_in[i][j]);
    for(j=0;j<NUM_OUTPUTS;++j)
      fscanf(fptr,"%f",&seed_out[i][j]);
  }
}

