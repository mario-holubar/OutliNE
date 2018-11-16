#ifndef SANE_H
#define SANE_H

#include <vector>
#include "QRandomGenerator"

//TODO Encapsulate!

class Neuron {
public:
    std::vector<double> w_in;
    std::vector<double> w_out;
    double value;
    float fitness;
    unsigned int n_genomes;

    Neuron();
};

class Genome {
public:
    unsigned int n_inputs, n_outputs;
    std::vector<Neuron *> genes;
    float fitness;

    Genome(unsigned int inputs, unsigned int outputs);
};

class NeuralNet {
public:
    NeuralNet();
    unsigned int n_inputs, n_outputs;
    std::vector<Neuron *> neurons;

    void from_genome(Genome g);
    double sigmoid(double x);
    std::vector<double> evaluate(std::vector<double> inputs);
};

class Pool {
public:
    unsigned int n_inputs, n_outputs, n_genomes, n_neurons;
    std::vector<Neuron> neurons;
    std::vector<Genome> genomes;

    Pool(unsigned int pop, unsigned int inputs, unsigned int outputs);

    void makeGenomes();
    void new_generation();
    void setFitness(unsigned int Genome, float fitness);
};

#endif // SANE_H
