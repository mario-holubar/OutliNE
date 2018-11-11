#include "sane.h"
#include <iostream>
#include "QDebug"

Neuron::Neuron() {

}

Genome::Genome(unsigned int inputs, unsigned int outputs) {
    n_inputs = inputs;
    n_outputs = outputs;
}

NeuralNet::NeuralNet() {

}

void NeuralNet::from_genome(Genome g) {
    n_inputs = g.n_inputs;
    n_outputs = g.n_outputs;
    for (unsigned int i = 0; i < g.genes.size(); i++) {
        neurons.push_back(g.genes[i]);
    }
}

double NeuralNet::sigmoid(double x) {
    return 2.0 / (1.0 + std::exp(-5.0 * x)) - 1;
}

std::vector<double> NeuralNet::evaluate(std::vector<double> inputs) {
    std::vector<double> outputs(n_outputs, 0.0);
    if (inputs.size() != n_inputs) {
        std::cerr << "Neural net input dimensions don't match. Expected " << n_inputs << ", got " << inputs.size();
        return outputs;
    }
    for (unsigned int n = 0; n < neurons.size(); n++) {
        double value = 0.0;
        for (unsigned int i = 0; i < neurons[n]->inputs.size(); i++) {
            auto c = neurons[n]->inputs[i];
            value += inputs[c.first] * c.second;
        }
        for (unsigned int i = 0; i < neurons[n]->outputs.size(); i++) {
            auto c = neurons[n]->outputs[i];
            outputs[c.first] += value * c.second;
        }
    }
    for (unsigned int i = 0; i < n_outputs; i++) {
        outputs[i] = sigmoid(outputs[i]);
    }
    return outputs;
}

Pool::Pool(unsigned int pop, unsigned int inputs, unsigned int outputs) {
    this->pop = pop;
    this->inputs = inputs;
    this->outputs = outputs;
    this->biases = 1;
    //rand.seed(unsigned(qrand()));

    // Generate random
    for (unsigned int i = 0; i < pop; i++) {//pop?
        Neuron n;
        for (int i = 0; i < 2; i++) {
            unsigned int index = rand.generate() % inputs;
            double weight = double(rand.generate()) / UINT32_MAX * 2 - 1;
            n.inputs.push_back(std::make_pair(index, weight));
            index = rand.generate() % outputs;
            weight = double(rand.generate()) / UINT32_MAX * 2 - 1;
            n.outputs.push_back(std::make_pair(index, weight));
        }
        neurons.push_back(n);
    }

    makeGenomes();
}

void Pool::makeGenomes() {
    genomes.clear();
    for (unsigned int i = 0; i < pop; i++) {//pop?
        Genome g(inputs, outputs);
        for(int j = 0; j < 4; j++) {
            unsigned int index = rand.generate() % pop;
            neurons[index].n_genomes++;
            g.genes.push_back(&neurons[index]);
        }
        genomes.push_back(g);
    }
}

bool fitnessSort(Neuron a, Neuron b) {
    return a.fitness > b.fitness;
}

void Pool::new_generation() {
    // Normalize fitnesses
    for (unsigned int i = 0; i < pop; i++) {//pop?
        neurons[i].fitness /= neurons[i].n_genomes;
    }
    // Only keep top 50%
    sort(neurons.begin(), neurons.end(), fitnessSort);
    for (unsigned int i = 0; i < pop / 2; i++) neurons.pop_back();

    // Crossover
    std::vector<Neuron> newNeurons(pop);
    for (unsigned int p1 = 0; p1 < pop / 2; p1++) {//pop?
        unsigned int p2 = rand.generate() % (p1 + 1); // p2 is equal or better than p1
        Neuron c1, c2;
        for (unsigned int i = 0; i < 2; i++) {
            if (rand.generate() < 0.5) {
                c1.inputs.push_back(neurons[p1].inputs[i]);
                c2.inputs.push_back(neurons[p2].inputs[i]);
            }
            else {
                c1.inputs.push_back(neurons[p2].inputs[i]);
                c2.inputs.push_back(neurons[p1].inputs[i]);
            }
            if (rand.generate() < 0.5) {
                c1.outputs.push_back(neurons[p1].outputs[i]);
                c2.outputs.push_back(neurons[p2].outputs[i]);
            }
            else {
                c1.outputs.push_back(neurons[p2].outputs[i]);
                c2.outputs.push_back(neurons[p1].outputs[i]);
            }
        }
        newNeurons[p1 * 2] = c1;
        newNeurons[p1 * 2 + 1] = c2;
    }

    neurons.clear();
    neurons = newNeurons;
}

void Pool::setFitness(unsigned int genome, float fitness) {
    for (unsigned int i = 0; i < genomes[genome].genes.size(); i++) {
        genomes[genome].genes[i]->fitness += fitness;
    }
}
