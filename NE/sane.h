#ifndef SANE_H
#define SANE_H

#include <vector>
#include "QRandomGenerator"
#include "UI/paramdialog.h"

class SANEParams {
public:
    unsigned n_inputs = 0;
    unsigned n_outputs = 0;

    unsigned tMax = 240;
    unsigned n_neurons = 16; //NE population
    unsigned n_genomes = 64; //cars
    unsigned neuronsPerGenome = 6;
    float initialWeightVariance = 0.5f;
    float mutationNoiseVariance = 0.01f;
    float sigmoidSteepness = 4.0f;
    unsigned tournamentSize = 3; //selection pressure

    SANEParams(unsigned inputs, unsigned outputs);
    void paramDialog(ParamDialog *d);
};

class SANENeuron {
public:
    std::vector<double> w_in;
    std::vector<double> w_out;
    double value;
    float fitness = 0.0f;
    unsigned n_genomes = 0;
};

class SANEGenome {
public:
    SANEParams *params;
    std::vector<SANENeuron *> genes;
    float fitness;

    SANEGenome(SANEParams *p);
};

class SANENeuralNet {
private:
    friend class Experiment;
    SANEParams *params;
    std::vector<SANENeuron *> neurons;
public:
    void from_genome(SANEGenome g);
    double sigmoid(double x);
    std::vector<double> evaluate(std::vector<double> inputs);
};

class SANEPool {
private:
    SANEParams *params;
    std::vector<SANENeuron> neurons;
    std::vector<SANEGenome> genomes;
    QRandomGenerator rand;
public:
    SANEPool();
    void makeNeurons(bool reset);
    void init(SANEParams *p);
    void makeGenomes();
    void newGeneration();
    void setFitness(unsigned Genome, float fitness);
    SANEGenome getGenome(unsigned i) {return genomes[i];}
};

#endif // SANE_H
