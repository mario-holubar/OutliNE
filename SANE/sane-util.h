
/*** sane-util.h
     David Moriarty
     May 1996

     Header files for sane-util.c
    
     ***/

void reseed(int val);
void create_pop(neuron *new_pop[]);
void load_pop(neuron *pop[],char *fname);
void load_partial(neuron *pop[],char *fname);
void save_pop(neuron *pop[], char *fname);
void save_partial(neuron**, char *fname);
void fm_output(neuron *pop[], char *fname);
void cluster_output(neuron *pop[],char *fname1, char *fname2,int size);
float Phi(neuron *pop[]);
int hamming(float *gene1, float *gene2, int size);
void print_net(network*);
float sgn(float);
double normal_dist(double,double);
