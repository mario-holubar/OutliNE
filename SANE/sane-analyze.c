
/*** sane-analyze.c
     David Moriarty
     April 1995

     This file contains code for analyzing neurons in the population

     To use this code, you need to have the following function defined:

     evaluate_network(net) - takes a pointer to a network and returns an
                             integer evalation.
     ***/

/** individual_capability_test will test how good of a solution each
  neuron forms on its own.  It currently tests the first NUM_HIDDEN neurons
  in the population.
  **/

individual_capability_test(pop)
  neuron *pop[];
{
  int i,j,score[NUM_HIDDEN];
  network net;

  /* set all neuron inputs to nil, and turn off all outputs */
  for(i=0;i<NUM_OUTPUTS;++i) {
    net.output_unit[i].numin = 0;
    net.output_unit[i].output = 0;
    net.output_unit[i].sigout = 0.0;
  }
  for(i=0;i<NUM_HIDDEN;++i) {
    pop[i]->decoded = 1; 
    for (j=0;j<GENE_SIZE/24;++j)  /*assumes 24 bit connection def.*/
      pop[i]->weight[j] = 0.0;    /*clear all weights*/
    net.hidden_unit[i].numin = 0;
    net.hidden_unit[i].output = 0;
    net.hidden_unit[i].sigout = 0.0;
  }

  /* Here's a little trick to only have one hidden neuron in a
  network: mark all of the other neurons as decoded and clear all the
  weights in their neuron structures.  Since all other connections are
  nil and only one neuron is to be decoded, only one hidden neuron
  will be used. */

  for(i=0;i<NUM_HIDDEN;++i) {
    pop[i]->decoded = 0;
    build_net(&net,pop);
    score[i] = evaluate_network(&net);
    for (j=0;j<GENE_SIZE/24;++j)  /*assumes 24 bit connection def.*/
      pop[i]->weight[j] = 0.0;    /*reset weights*/    
  }

  for(i=0;i<NUM_HIDDEN;++i)
    printf("Neuron %d Alone: %d\n",i,score[i]);
}

/** individual_necessity_test will see how necessary each neuron is to
  the subpopulation.  Each neuron is deleted from the subpopulation
  and the resulting network is tested.
  **/

individual_necessity_test(pop)
  neuron *pop[];
{
  int i,j,score[NUM_HIDDEN];
  network net;

  for(i=0;i<NUM_HIDDEN;++i)
    pop[i]->decoded = 0;

  /*similar trick as capability test: Mark neuron to be deleted as
    decoded and clear its weights.
    */

  for(i=0;i<NUM_HIDDEN;++i) {
    pop[i]->decoded = 1;
    for (j=0;j<GENE_SIZE/24;++j)  /*assumes 24 bit connection def.*/
      pop[i]->weight[j] = 0.0;    /*reset weights*/    
    build_net(&net,pop);
    score[i] = evaluate_network(&net);
    pop[i]->decoded = 0; /*Important: to get the real weights next time*/
  }

  for(i=0;i<NUM_HIDDEN;++i)
    printf("Network Minus Neuron %d: %d\n",i,score[i]);
}
