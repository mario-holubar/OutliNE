#ifndef ESP_H
#define ESP_H

#include <vector>
#include "QRandomGenerator"
#include "UI/paramdialog.h"

class ESPParams {
public:
    unsigned n_inputs = 0;
    unsigned n_outputs = 0;

    unsigned tMax = 240;
    unsigned neuronsPerSubpopulation = 6;
    unsigned n_genomes = 64; //cars
    unsigned subpopulationsPerGenome = 4;
    float initialWeightVariance = 0.5f;
    float mutationNoiseVariance = 0.1f;
    float sigmoidSteepness = 4.0f;
    unsigned tournamentSize = 2; //selection pressure

    ESPParams(unsigned inputs, unsigned outputs);
    void paramDialog(ParamDialog *d);
};

class ESPNeuron {
public:
    std::vector<double> w_in;
    std::vector<double> w_out;
    double value;
    float fitness = 0.0f;
    unsigned n_genomes = 0;
};

class ESPGenome {
public:
    ESPParams *params;
    std::vector<ESPNeuron *> genes;
    float fitness;

    ESPGenome(ESPParams *p);
};

class ESPNeuralNet {
private:
    friend class Experiment;
    ESPParams *params;
    std::vector<ESPNeuron *> neurons;
public:
    void from_genome(ESPGenome g);
    double sigmoid(double x);
    std::vector<double> evaluate(std::vector<double> inputs);
};

class ESPPool {
private:
    ESPParams *params;
    std::vector<std::vector<ESPNeuron> *> neurons;
    std::vector<ESPGenome> genomes;
    QRandomGenerator rand;
public:
    ESPPool();
    void makeNeurons(bool reset);
    void init(ESPParams *p);
    void makeGenomes();
    void newGeneration();
    void setFitness(unsigned Genome, float fitness);
    ESPGenome getGenome(unsigned i) {return genomes[i];}
};

#endif // ESP_H
