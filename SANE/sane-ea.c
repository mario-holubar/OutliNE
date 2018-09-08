
/*** sane-ga.c v. 2.0
     David E. Moriarty
     April 10, 1997

     This file contains common genetic algorithm functions for use
     with SANE 2.0.  

     ***/

#include "sane.h"
#include "sane-ea.h"
#include "sane-util.h"
#include "sane-seed.h"

/* need the number of breeding operations per ELITE neuron*/
#define OP_PER_ELITE    NUM_BREED / ELITE

/** evolve is the main genetic function.  After the population is
    evaluated, the top ELITE neurons are bred with themselves
    creating NUM_BREED * 2 new neurons that replace the worst neurons.
    The population is then mutated. and tested to obtain their
    fitness.  They are then sorted by their fitness level.

    A network-level evolution has also been added.  The best networks
    are kept in the structure best_nets and are sorted, bred, and
    mutated like the neurons.
    **/

void evolve(pop,cycles)
  neuron *pop[];  /*a population of neurons*/
  int cycles;       /*the number of generations to evolve*/
{
  FILE *fptr;
  int i,j;

  generation = 0;

  /* initialize best_nets structure */
  for(i=0;i<NUM_TRIALS;++i) {
    best_nets[i] = &actual_best_nets[i];
    for(j=0;j<NUM_HIDDEN;++j)
      best_nets[i]->neuron[j] = pop[randint(0,POP_SIZE-1)];
  }

  if (SEED_FLAG)
   seed_pop(pop);

  /*cycle population*/
  for (generation=0;generation<cycles;++generation) {

     for(j=0;j<POP_SIZE;++j) /*reset decoded flag*/
       pop[j]->decoded = 0;
     eval_pop(pop); 
     if (LAMARCKIAN) {  /*if we are doing Lamarckian evolution write back*/
       for(j=0;j<POP_SIZE;++j)
         if (pop[j]->decoded) 
           weights_to_gene(pop[j]);
      }

     qsort_neurons(pop,0,POP_SIZE-1);   /*sort neurons*/
     for(j=0;j<POP_SIZE;++j)
       pop[j]->ranking = j;  /*assign ranking, necessary for net. cross*/

     for (j=0;j<NUM_BREED;++j) /*mate with any neuron*/
      one_pt_crossover(pop[j%ELITE],pop[find_mate(j%ELITE)],
                       pop[POP_SIZE-(1+j*2)],pop[POP_SIZE-(2+j*2)]);  


     /* mutate neuron population */
     for (j=NUM_BREED;j<POP_SIZE;++j) 
       for(i=0;i<GENE_SIZE;++i) {
         if (i%2) {
           if (randint(0,100) < (MUT_RATE*2)) 
	     if (randint(0,20) == 0)
               pop[j]->gene[i] = -pop[j]->gene[i];
             else
               pop[j]->gene[i] = normal_dist(pop[j]->gene[i],1.0);
	 }
         else if (randint(0,100) < (MUT_RATE))
           pop[j]->gene[i] = randint(0,NUM_INPUTS+NUM_OUTPUTS-1);
       }

     sort_best_networks(); 
     /*breed network population*/
     for (j=0;j<TOP_NETS_BREED;++j)  
       new_network_one_pt_crossover(j,pop); 

     /*mutate network structure*/
     for(j=TOP_NETS_BREED;j<TOP_NETS;++j)
       for(i=0;i<NUM_HIDDEN;++i)
         if (randint(0,1000) < MUT_RATE)
           best_nets[j]->neuron[i] = pop[randint(0,POP_SIZE-1)];

     /*if at SAVE_CYCLE, save population file and print status to look*/

     if (!(generation % SAVE_CYCLE)) {
        save_pop(pop,savefile); 
        fptr = fopen("look","a");
        fprintf(fptr,"cycle: %d",generation);
        fprintf(fptr," top 5: %.2f,%.2f",pop[0]->fitness,pop[1]->fitness);
        fprintf(fptr,",%.2f,%.2f,%.2f",pop[2]->fitness,pop[3]->fitness,
                                 pop[4]->fitness);
        fprintf(fptr,"\n");
        fclose(fptr);
      }
   }
}

/** find_mate returns a mate for a given neuron.  The mate must have
  a fitness level greater  than the mating neuron (Except the top
  neuron).  

  This function is very aggressive and it might be better to just
  return another random neuron in the ELITE population.
    **/

int find_mate(num)
  int num;
{
  if (num == 0)   return randint(0,ELITE);
  else      return randint(0,num-1);
}

/** one_pt_crossover mates two parents together creating two children.
    The children's genes are taken from one crossover point of the
    parents.  SANE appears to perform the best with a one point
    crossover.
    **/

void one_pt_crossover(parent1,parent2,child1,child2)
  neuron *parent1,*parent2,*child1,*child2;
{
  int cross1;
  neuron *temp;
  register int i;

    /*find crossover point*/
    cross1 = randint(0,GENE_SIZE-1);

    /*randomize child positions.  This is important for the network level*/
    if (randbit()) {
      temp = child1;
      child1 = child2;
      child2 = temp;
    } 
    for (i=0;i<cross1;++i) {
       child1->gene[i] = parent1->gene[i];
       child2->gene[i] = parent2->gene[i];
     }
    for (i=cross1;i<GENE_SIZE;++i) {
       child1->gene[i] = parent2->gene[i];
       child2->gene[i] = parent1->gene[i];
     }
    if (randbit())
      for (i=0;i<GENE_SIZE;++i) 
       child1->gene[i] = parent1->gene[i];
    else
      for (i=0;i<GENE_SIZE;++i) 
       child2->gene[i] = parent1->gene[i];
}

/** two_pt_crossover mates two parents together creating two children.
    The children's genes are taken from two crossover points of the
    parents.  
    **/

void two_pt_crossover(parent1,parent2,child1,child2)
  neuron *parent1,*parent2,*child1,*child2;
{
  int cross1,cross2;
  register int i;

  /*find crossover points*/

  cross1 = lrand48()%GENE_SIZE;
  cross2 = lrand48()%GENE_SIZE; 
  if (cross2 < cross1) {
    i = cross1;
    cross1 = cross2;
    cross2 = i;
  } 

  for (i=0;i<cross1;++i) {
     child1->gene[i] = parent1->gene[i];
     child2->gene[i] = parent2->gene[i];
   }
  for (i=cross1;i<cross2;++i) {
     child1->gene[i] = parent2->gene[i];
     child2->gene[i] = parent1->gene[i];
   }
  for (i=cross2;i<GENE_SIZE;++i) {
     child1->gene[i] = parent1->gene[i];
     child2->gene[i] = parent2->gene[i];
   }
}

/** network_one_pt_crossover will perform a crossover operation on two
    networks.  Each network is simply an array of pointers to neurons so
    the crossover operation merely exchanges neurons.  
 
    When assigning neurons to the new networks, if any neuron is a
    "breeding" neuron, there is an equal probability that either
    itself, or one of its children will be assigned.  This allows new
    network to point to offspring neurons.  This serves as the
    mutation operator in the network population.

    **/

void network_one_pt_crossover(which,pop)
 int which;
 neuron *pop[];
{
  int cross1,mate,child1,child2;
  int i,j;

  if (which) mate = randint(0,which-1);
  else       mate = 0; 

  child1 = TOP_NETS-(1+which*2);
  child2 = TOP_NETS-(2+which*2);

  /*find crossover point*/
  cross1 = randint(0,NUM_HIDDEN-1);

  for (i=0;i<cross1;++i) {
    j = best_nets[which]->neuron[i]->ranking;
    if ((j < NUM_BREED) & randint(0,1))
      best_nets[child1]->neuron[i] = pop[POP_SIZE-((randbit()+1)+j*2)];
    else
      best_nets[child1]->neuron[i] = pop[j]; 
    j = best_nets[mate]->neuron[i]->ranking;
    if ((j < NUM_BREED) & randint(0,1))
      best_nets[child2]->neuron[i] = pop[POP_SIZE-((randbit()+1)+j*2)];
    else
      best_nets[child2]->neuron[i] = pop[j]; 
    
   }
  for (i=cross1;i<NUM_HIDDEN;++i) {
    j = best_nets[which]->neuron[i]->ranking;
    if ((j < NUM_BREED) & randint(0,1))
      best_nets[child2]->neuron[i] = pop[POP_SIZE-((randbit()+1)+j*2)];
    else
      best_nets[child2]->neuron[i] = pop[j]; 
    j = best_nets[mate]->neuron[i]->ranking;
    if ((j < NUM_BREED) & randint(0,1))
      best_nets[child1]->neuron[i] = pop[POP_SIZE-((randbit()+1)+j*2)];
    else
      best_nets[child1]->neuron[i] = pop[j]; 
  }

  if (NUM_HIDDEN > 10) {
    best_nets[child1]->neuron[randint(0,NUM_HIDDEN-1)] = pop[randint(0,POP_SIZE-1)];
    best_nets[child2]->neuron[randint(0,NUM_HIDDEN-1)] = pop[randint(0,POP_SIZE-1)];
  }

/* below is some code to do a simple network crossover without
   checking if a neuron is a breeding neuron. */
/*
  for (i=0;i<cross1;++i) {
    best_nets[child1]->neuron[i] = best_nets[which]->neuron[i];
    best_nets[child2]->neuron[i] = best_nets[mate]->neuron[i];
   }
  for (i=cross1;i<NUM_HIDDEN;++i) {
    best_nets[child1]->neuron[i] = best_nets[mate]->neuron[i];
    best_nets[child2]->neuron[i] = best_nets[which]->neuron[i];
   }
*/
}

/** new_network_one_pt_crossover is just like network_one_pt_crossover
    except that it uses only knowlege of the ELITE parameter to
    reassign the neuron pointers.  If ELITE < NUM_BREED neurons in the
    top ELITE will have more than one breeding operation.
    **/

void new_network_one_pt_crossover(which,pop)
 int which;
 neuron *pop[];
{
  int cross1,mate,child1,child2;
  int i,j;

  if (which) mate = randint(0,which-1);
  else       mate = 0; 

  child1 = TOP_NETS-(1+which*2);
  child2 = TOP_NETS-(2+which*2);

  /*find crossover point*/
  cross1 = randint(0,NUM_HIDDEN-1);

  for (i=0;i<cross1;++i) {
    j = best_nets[which]->neuron[i]->ranking;
    if ((j < ELITE) & !randint(0,1))
      best_nets[child1]->neuron[i] = pop[POP_SIZE-((randbit()+1)+
                                         j*randint(1,OP_PER_ELITE)*2)];
    else
      best_nets[child1]->neuron[i] = pop[j]; 
    j = best_nets[mate]->neuron[i]->ranking;
    if ((j < ELITE) & !(randint(0,1)))
      best_nets[child2]->neuron[i] = pop[POP_SIZE-((randbit()+1)+
                                         j*randint(1,OP_PER_ELITE)*2)];
    else
      best_nets[child2]->neuron[i] = pop[j]; 
    
   }
  for (i=cross1;i<NUM_HIDDEN;++i) {
    j = best_nets[which]->neuron[i]->ranking;
    if ((j < ELITE) & !(randint(0,1)))
      best_nets[child2]->neuron[i] = pop[POP_SIZE-((randbit()+1)+
                                         j*randint(1,OP_PER_ELITE)*2)];
    else
      best_nets[child2]->neuron[i] = pop[j]; 
    j = best_nets[mate]->neuron[i]->ranking;
    if ((j < ELITE) & !(randint(0,1)))
      best_nets[child1]->neuron[i] = pop[POP_SIZE-((randbit()+1)+
                                         j*randint(1,OP_PER_ELITE)*2)];
    else
      best_nets[child1]->neuron[i] = pop[j]; 
  }

  if (NUM_HIDDEN > 10) {
    best_nets[child1]->neuron[randint(0,NUM_HIDDEN-1)] = pop[randint(0,POP_SIZE-1)];
    best_nets[child2]->neuron[randint(0,NUM_HIDDEN-1)] = pop[randint(0,POP_SIZE-1)];
  }

/* below is some code to do a simple network crossover without
   checking if a neuron is a breeding neuron. */
/*
  for (i=0;i<cross1;++i) {
    best_nets[child1]->neuron[i] = best_nets[which]->neuron[i];
    best_nets[child2]->neuron[i] = best_nets[mate]->neuron[i];
   }
  for (i=cross1;i<NUM_HIDDEN;++i) {
    best_nets[child1]->neuron[i] = best_nets[mate]->neuron[i];
    best_nets[child2]->neuron[i] = best_nets[which]->neuron[i];
   }
*/
}

/* network pertubation does not perform crossover, but instead
pertubates two networks.  Pertubations is done as in
network_one_pt_crossover.  */

void network_pertubation(which,pop)
  int which;
  neuron *pop[];
{

  int child1,child2,mate;
  int i,j;
  
  if (which) mate = randint(0,which-1);
  else       mate = 0; 

  child1 = TOP_NETS-(1+which*2);
  child2 = TOP_NETS-(2+which*2);

  for(i=0;i<NUM_HIDDEN;++i) {
    j = best_nets[which]->neuron[i]->ranking;
    if ((j < NUM_BREED) & randint(0,1))
      best_nets[child1]->neuron[i] = pop[POP_SIZE-((randbit()+1)+j*2)];
    else
      best_nets[child1]->neuron[i] = pop[j]; 
  }

  for(i=0;i<NUM_HIDDEN;++i) {
    j = best_nets[mate]->neuron[i]->ranking;
    if ((j < NUM_BREED) & randint(0,1))
      best_nets[child2]->neuron[i] = pop[POP_SIZE-((randbit()+1)+j*2)];
    else
      best_nets[child2]->neuron[i] = pop[j]; 
  }
}

/** new_network_level is an experimental non-crossover method of
    searching for new network level structures.  
    **/

void new_network_level(pop)
  neuron *pop[];
{
  neuron *new_nets[TOP_NETS][NUM_HIDDEN];
  int i,j,rank;
  int player1,player2,winner;

 for(i=TOP_NETS_BREED;i<TOP_NETS;++i) 
    for(j=0;j<NUM_HIDDEN;++j) {
      player1 = randint(0,TOP_NETS_BREED);
      player2 = randint(0,TOP_NETS_BREED);
      winner = min(player1,player2);
      player1 = randint(0,TOP_NETS_BREED);
      winner = min(winner,player1);
      player1 = randint(0,TOP_NETS_BREED);
      winner = min(winner,player1);

      rank = best_nets[winner]->neuron[j]->ranking;
      if ((rank < NUM_BREED) & randint(0,1))
        new_nets[i][j] = pop[POP_SIZE-((randbit()+1)+rank*2)];
      else  
        new_nets[i][j] = best_nets[winner]->neuron[j];
    }

  for(i=TOP_NETS_BREED;i<TOP_NETS;++i)
    for(j=0;j<NUM_HIDDEN;++j)
      best_nets[i]->neuron[j] = new_nets[i][j];
}

/** insertion sort the neurons by their fitness value.
    **/

void sort_neurons(pop)
  neuron *pop[];
{
  int i,j;
  neuron *temp;

  for (i=0;i<POP_SIZE;++i)
     for (j=POP_SIZE-1;j>i;--j)
        if (pop[j]->fitness > pop[j-1]->fitness) {
           temp = pop[j];
           pop[j] = pop[j-1];
           pop[j-1] = temp;
         }
}

/** insertion sort for the best networks structure.
    **/

void sort_best_networks()
{
  int i,j;
  
  best_net_structure *temp;

  for(i=0;i<NUM_TRIALS;++i)
    for(j=NUM_TRIALS-1;j>i;--j)
      if (best_nets[j]->fitness > best_nets[j-1]->fitness) {
         temp = best_nets[j];
         best_nets[j] = best_nets[j-1];
         best_nets[j-1] = temp;
     } 
}

/*** quick sort the neurons.  Use for large populations (>50)
     ***/

void qsort_neurons(pop,p,r)
  neuron *pop[];
  int p,r;
{
  int q;
  if (p<r) {
    q = qpartition_neurons(pop,p,r);
    qsort_neurons(pop,p,q);
    qsort_neurons(pop,q+1,r);
  }
}

/*** partition function for qsort.
     ***/

int qpartition_neurons(pop,p,r)
  neuron *pop[];
  int p,r;
{
  register int i,j;
  neuron *x,*temp;

  x = pop[p];
  i = p - 1;
  j = r + 1;
  while(1) {
    do{
      --j;
    }while(pop[j]->fitness < x->fitness);
    do{
      ++i;
    }while(pop[i]->fitness > x->fitness);
    if (i < j) {
      temp = pop[i];
      pop[i] = pop[j];
      pop[j] = temp;
    }
    else
      return j;
  }
}
