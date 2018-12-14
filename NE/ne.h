#ifndef NE_H
#define NE_H

#include <vector>
#include "UI/paramdialog.h"
#include "QRandomGenerator"

class Neuron {
public:
    Neuron();

    std::vector<double> w_in;
    std::vector<double> w_out;
    double value;
};

class NeuralNet {
private:
    std::vector<Neuron> neurons;
public:
    void addNeuron(Neuron n);
    Neuron *getNeuron(unsigned neuron);
    unsigned numberOfNeurons();
    double sigmoid(double x);
    std::vector<double> evaluate(std::vector<double> inputs);
};

class NE {
protected:
    std::vector<NeuralNet> nets;
    QRandomGenerator rand;
public:
    static unsigned n_genomes;
    static float sigmoidSteepness;
    static float initialWeightVariance;
    static float mutationNoiseVariance;
    static unsigned tournamentSize;

    static unsigned n_inputs;
    static unsigned n_outputs;

    NE();
    virtual ~NE();
    NeuralNet *getNet(unsigned net);

    virtual void paramDialog(ParamDialog *d) = 0;
    virtual void init(bool reset) = 0;
    virtual void makeGenomes() = 0; //make nets too!
    virtual void setFitness(unsigned genome, float fitness) = 0;
    virtual void newGeneration() = 0;
};

#endif // NE_H
