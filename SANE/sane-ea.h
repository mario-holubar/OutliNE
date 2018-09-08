
void evolve(neuron**, int);
int find_mate(int);
void one_pt_crossover(neuron*, neuron*, neuron*, neuron*);
void two_pt_crossover(neuron*, neuron*, neuron*, neuron*);
void network_one_pt_crossover(int, neuron**);
void new_network_one_pt_crossover(int, neuron**);
void network_pertubation(int, neuron**);
void new_network_level(neuron**);
void sort_neurons(neuron**);
void sort_best_networks();
void qsort_neurons(neuron**,int,int);
int qpartition_neurons(neuron**,int,int);


