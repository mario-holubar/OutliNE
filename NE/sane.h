#ifndef SANE_H
#define SANE_H

#include <vector>
#include "QRandomGenerator"

class SANEParams {
public:
    unsigned int tMax = 240;
    unsigned int n_inputs = 0;
    unsigned int n_outputs = 0;
    unsigned int n_neurons = 64; //NE population
    unsigned int n_genomes = 64; //cars
    unsigned int neuronsPerGenome = 6;
    float initialWeightVariance = 0.5f;
    float mutationNoiseVariance = 0.1f;
    float sigmoidSteepness = 4.0f;
    unsigned int tournamentSize = 5; //selection pressure

    SANEParams(unsigned int inputs, unsigned int outputs);
};

class Neuron {
public:
    std::vector<double> w_in;
    std::vector<double> w_out;
    double value;
    float fitness = 0.0f;
    unsigned int n_genomes;
};

class Genome {
public:
    SANEParams *params;
    std::vector<Neuron *> genes;
    float fitness;

    Genome(SANEParams *p);
};

class NeuralNet {
private:
    friend class Experiment;
    SANEParams *params;
    std::vector<Neuron *> neurons;
public:
    void from_genome(Genome g);
    double sigmoid(double x);
    std::vector<double> evaluate(std::vector<double> inputs);
};

class Pool {
private:
    SANEParams *params;
    std::vector<Neuron> neurons;
    std::vector<Genome> genomes;
public:
    Pool(SANEParams *p);

    void makeGenomes();
    void new_generation();
    void setFitness(unsigned int Genome, float fitness);

    Genome getGenome(unsigned int i) {return genomes[i];}
};

#endif // SANE_H
