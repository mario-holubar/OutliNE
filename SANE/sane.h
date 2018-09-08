
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*** sane.h for SANE 2.0
     David E. Moriarty
     May 1997

     This file contains global constants and structure definitions for
     SANE.

     To run only a neuron-level evolution, set TOP_NETS_BREED to 0.

     ***/

#define SAVE_CYCLE 25        /*how often the population is saved*/

#define GENE_SIZE        24  /*number of connections per neuron*2*/
#define POP_SIZE       1000  /*total number of neurons*/
#define NUM_HIDDEN       12  /*number of neurons used to make a network*/
#define NUM_BREED       200  /*the top number of neurons that reproduce*/
#define ELITE           200
#define MUT_RATE          2  /*mutation rate in % 10 = 10%*/
#define NUM_TRIALS      100  /*the number of networks formed per generation.
                               This should be >= to TOP_NETS*/  

/* These next definitions pertain to the network level evolution*/
#define TOP_NETS        100  /*the # of networks kept track of per generation*/
#define TOP_NETS_BREED   20  /*the # of networks to breed. < 1/3*TOP_NETS */

/* here are some definitions for our neural networks */
#define NUM_INPUTS        2   /*number of input units*/
#define NUM_OUTPUTS       1   /*number of output units*/
#define MAX_CONNECTIONS 400   /*max # of connections any unit may have*/

/* These next definitions are for the local learning option.  The
   CRITIC parameter will fire up an AHC to train the networks.  I
   haven't had any success with this yet.  The LOCAL_LEARN function
   will fire up a performance difference based learning method that
   seems to work quite well for certain domains.  Pay attention to
   whether you are maximizing or minimizing your rewards.*/

#define ETA              0.1
#define ALPHA            0.9
#define DECAY_ETA        0
#define CRITIC           0  /*use an AHC to train networks*/
#define LOCAL_LEARN      0  /*use your own learning rule to train*/
#define LAMARCKIAN       0  /*write learned info back to chromosome?*/
#define LL_LOWER_LEVEL -10  /*the performance drops or increases at which*/
#define LL_UPPER_LEVEL  10  /*punishment or reinforcment is given.*/
#define MAXIMIZE_R       0  /*trying to max (1) or min (0) the reward?*/
#define LOCAL_STEPS    500  /*this defines the number of actions rewards should
                              be evaluated over for local_learning.*/

/* here are some definitions for the population seeding routines */
#define SEED_FLAG           0    /*flag to set if you want to seed*/
#define NUM_SEED_EXAMPLES   100  /*the number of examples in seed.txt*/
#define SEED_NETS           25   /*the number of networks to seed*/
#define SEED_TRIALS         100   /*the number of bp learning cycles per net*/

/* here are some useful macros */
#define min(x, y)               ((x <= y) ? x : y)
#define max(x, y)               ((x >= y) ? x : y)
#define randbit()               (lrand48()%2)
#define randint(x,y)            (lrand48()%(y-x+1)+x)
#define randfloat()             (lrand48()/2147483648.0)

/* neuron genetic stucture */

typedef struct {
   float gene[GENE_SIZE];       /*neuron's chromsome*/
   int in_conn[GENE_SIZE/2],out_conn[GENE_SIZE/2];
   int numin, numout;
   float in_weight[GENE_SIZE/2],out_weight[GENE_SIZE/2];
   float in_delta[GENE_SIZE/2],out_delta[GENE_SIZE/2];
   char decoded;               /*has neuron already been decoded?*/
   float fitness;              /*neuron's fitness value*/
   int tests;                  /*holds the # of networks participated in*/
   int ranking;                /*where neuron ranks in population*/
   int type;
   double sum;
   float sigout;
   char output;
   float error;
 } neuron;

/*network definition*/

typedef struct {
  float input[NUM_INPUTS];
  neuron *hidden[NUM_HIDDEN];
  float sigout[NUM_OUTPUTS];
  float sum[NUM_OUTPUTS];
  int winner;  /*which output unit was highest*/
}network;

/* This next structure will keep pointers to the best networks of the
   previous generation.  This is the genetic description for the the
   network level evolution */

typedef struct {
  neuron *neuron[NUM_HIDDEN];
  float fitness;
} best_net_structure;


// changed into external definitions. -Bram

extern best_net_structure actual_best_nets[NUM_TRIALS];
extern best_net_structure *best_nets[NUM_TRIALS];

extern int generation;               /*global generation counter*/
extern char savefile[15];            /*what's the name of the population file*/



