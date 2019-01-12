#ifndef SANE_H
#define SANE_H

#include <vector>
#include "QRandomGenerator"
#include "UI/paramdialog.h"
#include "NE/ne.h"

class SANEGene {
public:
    Neuron neuron;
    float fitness = 0.0f;
    unsigned n_genomes = 0;

    SANEGene() {}
    SANEGene(Neuron n);
};

class SANEGenome {
public:
    std::vector<SANEGene *> genes;
};

class SANE : public NE {
public:
    unsigned n_neurons = 16;
    unsigned neuronsPerGenome = 4;

    std::vector<SANEGene> genes;
    std::vector<SANEGenome> genomes;

    SANE();

    void makeGenes();

    void paramDialog(ParamDialog *d);
    void init(bool reset);
    void makeGenomes();
    void setFitness(unsigned genome, float fitness);
    void newGeneration();
};

#endif // SANE_H
