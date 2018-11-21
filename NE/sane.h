//#ifndef SANE_H
//#define SANE_H

//#include <vector>
//#include "QRandomGenerator"
//#include "UI/paramdialog.h"

//class PoolParams {
//public:
//    unsigned n_inputs = 0;
//    unsigned n_outputs = 0;

//    unsigned tMax = 240;
//    unsigned n_neurons = 16; //NE population
//    unsigned n_genomes = 64; //cars
//    unsigned neuronsPerGenome = 6;
//    float initialWeightVariance = 0.5f;
//    float mutationNoiseVariance = 0.01f;
//    float sigmoidSteepness = 4.0f;
//    unsigned tournamentSize = 3; //selection pressure

//    PoolParams(unsigned inputs, unsigned outputs);
//    void paramDialog(ParamDialog *d);
//};

//class Neuron {
//public:
//    std::vector<double> w_in;
//    std::vector<double> w_out;
//    double value;
//    float fitness = 0.0f;
//    unsigned n_genomes = 0;
//};

//class Genome {
//public:
//    PoolParams *params;
//    std::vector<Neuron *> genes;
//    float fitness;

//    Genome(PoolParams *p);
//};

//class NeuralNet {
//private:
//    friend class Experiment;
//    PoolParams *params;
//    std::vector<Neuron *> neurons;
//public:
//    void from_genome(Genome g);
//    double sigmoid(double x);
//    std::vector<double> evaluate(std::vector<double> inputs);
//};

//class Pool {
//private:
//    PoolParams *params;
//    std::vector<Neuron> neurons;
//    std::vector<Genome> genomes;
//    QRandomGenerator rand;
//public:
//    Pool();
//    void makeNeurons(bool reset);
//    void init(PoolParams *p);
//    void makeGenomes();
//    void new_generation();
//    void setFitness(unsigned Genome, float fitness);
//    Genome getGenome(unsigned i) {return genomes[i];}
//};

//#endif // SANE_H
