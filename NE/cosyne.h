#ifndef COSYNE_H
#define COSYNE_H

#include <vector>
#include "QRandomGenerator"
#include "UI/paramdialog.h"

#include "NE/ne.h"

//redundant
class CosyneGene {
public:
    double weight;

    CosyneGene() {}
    CosyneGene(double w);
};

class CosyneGenome {
public:
    std::vector<CosyneGene *> genes;
    float fitness = 0.0f;
};

class Cosyne : public NE {
public:
    unsigned neuronsPerGenome = 4;

    std::vector<std::vector<CosyneGene>> genes;
    std::vector<CosyneGenome> genomes;

    Cosyne();

    unsigned subpopulationsPerGenome();
    void makeGenes();

    void paramDialog(ParamDialog *d);
    void init(bool reset);
    void makeGenomes();
    void setFitness(unsigned genome, float fitness);
    void newGeneration();
};

#endif // COSYNE_H
