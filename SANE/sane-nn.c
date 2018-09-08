/*** sane-nn.c v. 2.0
     David Moriarty
     April 1997

     This file contains the code for building, activating, and
     printing a neural network from a subpopulation of SANE neurons.

     ***/

#include "sane.h"
#include "sane-nn.h"

/**build_net builds a neural network from the subpopulation of neurons
   pointed to by pop.  The resulting network is defined in net.
   **/

void build_net(network *net, neuron *pop[])
{
  int i;



  for(i=0;i<NUM_HIDDEN;++i) {
    net->hidden[i] = pop[i];
    if (pop[i]->decoded == 0) 
      gene_to_weights(pop[i]);
  }
}

void gene_to_weights(neuron *n)
{
  int i;

  n->numin = 0;
  n->numout = 0;
  for(i=0;i<GENE_SIZE;i+=2) {
    if (n->gene[i]<NUM_INPUTS) {
      n->in_conn[n->numin] = (int) n->gene[i];
      n->in_delta[n->numin] = 0.0;
      n->in_weight[n->numin++] = n->gene[i+1];
    }
    else {
      n->out_conn[n->numout] = (int) n->gene[i] - NUM_INPUTS;
      n->out_delta[n->numin] = 0.0;
      n->out_weight[n->numout++] = n->gene[i+1];
    }
  }
  n->decoded = 1;
}

void weights_to_gene(neuron *n)
{
  int i,in,out;

  in = out = 0;
  for(i=0;i<GENE_SIZE;i+=2) {
    if (n->gene[i] >= NUM_INPUTS)
      n->gene[i+1] = n->out_weight[out++];
    else
      n->gene[i+1] = n->in_weight[in++];
  }
}

/** activate_net will activate the neural network.  The input layer
    must be setup before calling activate_net.
    **/

void activate_net(network *net)
{
   int i,j;
   
  
   double sum,max;
   neuron *h;

   max = -999999.0;

   /*reset output layer*/
   for(i=0;i<NUM_OUTPUTS;++i)
     net->sum[i] = 0.0;
 
   for (i=0;i<NUM_HIDDEN;++i) { 
      h = net->hidden[i];
      sum = 0.0;
      for(j=0;j<h->numin;++j) 
        sum += h->in_weight[j] * net->input[h->in_conn[j]];
      h->sigout =   1/(1+exp(-sum));
      for(j=0;j<h->numout;++j)
        net->sum[h->out_conn[j]] += h->out_weight[j]*h->sigout;
   }
   for(i=0;i<NUM_OUTPUTS;++i) {
     net->sigout[i] = 1/(1+exp(-net->sum[i]));
     if (net->sum[i] >= max) {
       net->winner = i;
       max = net->sum[i];
     }
   }
  
}

float sigmoid(float sum)
{
  return (1/(1+exp(-sum)));
}

float gauss(float sum,float sd)
{
  return (exp(-(sum*sum/(2*(sd*sd)))));
}



