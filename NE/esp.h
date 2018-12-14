#ifndef ESP_H
#define ESP_H

#include <vector>
#include "QRandomGenerator"
#include "UI/paramdialog.h"

#include "NE/ne.h"

class ESPGene {
public:
    Neuron neuron;
    float fitness = 0.0f;
    unsigned n_genomes = 0;

    ESPGene(Neuron n);
};

class ESPGenome {
public:
    std::vector<ESPGene *> genes;
};

class ESP : public NE {
public:
    unsigned neuronsPerSubpopulation = 6;
    unsigned subpopulationsPerGenome = 4;

    std::vector<std::vector<ESPGene>> genes;
    std::vector<ESPGenome> genomes;

    void makeGenes();

    void paramDialog(ParamDialog *d);
    void init(bool reset);
    void makeGenomes();
    void setFitness(unsigned genome, float fitness);
    void newGeneration();
};

#endif // ESP_H
