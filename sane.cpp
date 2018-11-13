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
    neurons.clear();
    for (unsigned int i = 0; i < g.genes.size(); i++) {
        neurons.push_back(g.genes[i]);
    }
}

double NeuralNet::sigmoid(double x) {
    return 2.0 / (1.0 + std::exp(-5.0 * x)) - 1;
}

std::vector<double> NeuralNet::evaluate(std::vector<double> inputs) {
    std::vector<double> outputs(n_outputs, 0.0);
    /*if (inputs.size() != n_inputs) {
        std::cerr << "Neural net input dimensions don't match. Expected " << n_inputs << ", got " << inputs.size();
        return outputs;
    }*/
    while (inputs.size() < n_inputs) inputs.push_back(1.0);
    for (unsigned int n = 0; n < neurons.size(); n++) {
        //double value = 0.0;
        neurons[n]->value = 0.0;
        for (unsigned int i = 0; i < n_inputs; i++) {
            neurons[n]->value += inputs[i] * neurons[n]->w_in[i];
        }
    }
    for (unsigned int n = 0; n < neurons.size(); n++) {
        neurons[n]->value = sigmoid(neurons[n]->value);
    }
    for (unsigned int n = 0; n < neurons.size(); n++) {
        for (unsigned int i = 0; i < n_outputs; i++) {
            outputs[i] += neurons[n]->value * neurons[n]->w_out[i];
        }
    }
    for (unsigned int i = 0; i < n_outputs; i++) {
        outputs[i] = sigmoid(outputs[i]);
    }
    return outputs;
}

Pool::Pool(unsigned int pop, unsigned int inputs, unsigned int outputs) {
    n_genomes = pop;
    n_inputs = inputs + 1; // 1 bias neuron
    n_outputs = outputs;
    n_neurons = 32;
    rand.seed(unsigned(qrand()));

    // Generate random neurons
    std::default_random_engine generator;
    std::normal_distribution<double> dist(0.0, 1.0);
    for (unsigned int i = 0; i < n_neurons; i++) {//pop?
        Neuron n;
        for (unsigned int i = 0; i < n_inputs; i++) {
            double weight = dist(generator);
            n.w_in.push_back(weight);
        }
        for (unsigned int i = 0; i < n_outputs; i++) {
            double weight = dist(generator);
            n.w_out.push_back(weight);
        }
        neurons.push_back(n);
    }

    makeGenomes();
}

void Pool::makeGenomes() {
    genomes.clear();
    for (unsigned int i = 0; i < n_genomes; i++) {
        Genome g(n_inputs, n_outputs);
        for(int j = 0; j < 8; j++) {
            unsigned int index = rand.generate() % n_neurons;
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
    for (unsigned int i = 0; i < n_neurons; i++) {//pop?
        neurons[i].fitness /= neurons[i].n_genomes;
    }
    // Only keep top 50%
    sort(neurons.begin(), neurons.end(), fitnessSort);
    for (unsigned int i = 0; i < n_neurons / 2; i++) neurons.pop_back();

    // Crossover
    for (unsigned int p1 = 0; p1 < n_neurons / 4; p1++) {//pop?
        unsigned int p2 = rand.generate() % (p1 + 1); // p2 is equal or better than p1
        Neuron c1, c2;
        for (unsigned int i = 0; i < n_inputs; i++) {
            if (double(rand.generate()) / INT32_MAX < 0.5) {
                c1.w_in.push_back(neurons[p1].w_in[i]);
                c2.w_in.push_back(neurons[p2].w_in[i]);
            }
            else {
                c1.w_in.push_back(neurons[p2].w_in[i]);
                c2.w_in.push_back(neurons[p1].w_in[i]);
            }
        }
        for (unsigned int i = 0; i < n_outputs; i++) {
            if (double(rand.generate()) / INT32_MAX < 0.5) {
                c1.w_out.push_back(neurons[p1].w_out[i]);
                c2.w_out.push_back(neurons[p2].w_out[i]);
            }
            else {
                c1.w_out.push_back(neurons[p2].w_out[i]);
                c2.w_out.push_back(neurons[p1].w_out[i]);
            }
        }
        neurons.push_back(c1);
        neurons.push_back(c2);
    }

    /*std::vector<Neuron> newNeurons;
    float total = 0;
    for (unsigned int i = 0; i < neurons.size(); i++) {
        total += neurons[i].fitness;
    }
    for (unsigned int p1 = 0; p1 < neurons.size() / 2; p1++) {
        float c = float(rand.generate()) / INT32_MAX * total;
        for (unsigned int p2 = 0; p2 < neurons.size(); p2++) {
            c -= neurons[p2].fitness;
            if (c < 0.0f) {
                total -= neurons[p2].fitness;
                break;
            }
        }

    }*/

    /*for (unsigned int i = 0; i < n_neurons / 2; i++) neurons.pop_back();
    for (unsigned int p1 = 0; p1 < n_neurons / 2; p1++) {//pop?
        unsigned int p2 = rand.generate() % (p1 + 1); // p2 is equal or better than p1
        Neuron c;
        for (unsigned int i = 0; i < neurons[p1].inputs.size(); i++) {
            c.inputs.push_back(neurons[p1].inputs[i]);
        }
        for (unsigned int i = 0; i < neurons[p2].inputs.size(); i++) {
            c.inputs.push_back(neurons[p2].inputs[i]);
        }
        for (unsigned int i = 0; i < neurons[p1].outputs.size(); i++) {
            c.outputs.push_back(neurons[p1].outputs[i]);
        }
        for (unsigned int i = 0; i < neurons[p2].outputs.size(); i++) {
            c.outputs.push_back(neurons[p2].outputs[i]);
        }
        neurons.push_back(c);
    }*/

    // Mutation
    for (unsigned int i = 0; i < n_neurons; i++) {
        for (unsigned int j = 0; j < n_inputs; j++) {
            if (double(rand.generate()) / INT32_MAX < 0.1) {
                double weight = double(rand.generate()) / UINT32_MAX * 2 - 1;
                neurons[i].w_in[j] = weight;
            }
        }
        for (unsigned int j = 0; j < n_outputs; j++) {
            if (double(rand.generate()) / INT32_MAX < 0.1) {
                double weight = double(rand.generate()) / UINT32_MAX * 2 - 1;
                neurons[i].w_out[j] = weight;
            }
        }
    }

    // Noise
    std::default_random_engine generator;
    std::normal_distribution<double> dist(0.0, 0.1);
    for (unsigned int i = 0; i < n_neurons; i++) {
        for (unsigned int j = 0; j < n_inputs; j++) {
            neurons[i].w_in[j] += dist(generator);
        }
        for (unsigned int j = 0; j < n_outputs; j++) {
            neurons[i].w_out[j] += dist(generator);
        }
    }
}

void Pool::setFitness(unsigned int genome, float fitness) {
    for (unsigned int i = 0; i < genomes[genome].genes.size(); i++) {
        genomes[genome].genes[i]->fitness += fitness;
    }
}
