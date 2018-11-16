#include "sane.h"
#include "QDebug"

Neuron::Neuron() {
    fitness = 0.0f;
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
    //sigmoid steepness (exp) controls how much outputs can change on small input changes
    //small: smoother but doesn't learn new things as quickly
    return 2.0 / (1.0 + std::exp(-4.0 * x)) - 1;
}

std::vector<double> NeuralNet::evaluate(std::vector<double> inputs) {
    std::vector<double> outputs(n_outputs, 0.0);
    while (inputs.size() < n_inputs) inputs.push_back(1.0);
    for (unsigned int n = 0; n < neurons.size(); n++) {
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
    n_inputs = inputs + 0; // 0 bias neurons
    n_outputs = outputs;
    n_neurons = 64;

    // Generate random neurons
    std::random_device rd;
    std::mt19937 generator {rd()};
    std::normal_distribution<double> dist(0.0, 0.5);
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
        for(int j = 0; j < 6; j++) {
            unsigned int index = unsigned(qrand()) % n_neurons;
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

    // Crossover
    std::vector<Neuron> newNeurons;
    Neuron p1, p2;
    unsigned int tournamentSize = 5; //selection pressure
    for (unsigned int n = 0; n < n_neurons / 2; n++) {
        // Tournament selection for both parents
        float bestFitness = -1.0f;
        unsigned int bestIndex = 0;
        for (unsigned int k = 0; k < tournamentSize; k++) {
            unsigned int c = unsigned(qrand()) % n_neurons;
            if (neurons[c].fitness > bestFitness) {
                bestFitness = neurons[c].fitness;
                bestIndex = c;
            }
        }
        p1 = neurons[bestIndex];
        bestFitness = -1.0f;
        bestIndex = 0;
        for (unsigned int k = 0; k < tournamentSize; k++) {
            unsigned int c = unsigned(qrand()) % n_neurons;
            if (neurons[c].fitness > bestFitness) {
                bestFitness = neurons[c].fitness;
                bestIndex = c;
            }
        }
        p2 = neurons[bestIndex];

        // Random crossover
        Neuron c1, c2;
        for (unsigned int i = 0; i < n_inputs; i++) {
            if (qrand() % 2) {
                c1.w_in.push_back(p1.w_in[i]);
                c2.w_in.push_back(p2.w_in[i]);
            }
            else {
                c1.w_in.push_back(p2.w_in[i]);
                c2.w_in.push_back(p1.w_in[i]);
            }
        }
        for (unsigned int i = 0; i < n_outputs; i++) {
            if (qrand() % 2) {
                c1.w_out.push_back(p1.w_out[i]);
                c2.w_out.push_back(p2.w_out[i]);
            }
            else {
                c1.w_out.push_back(p2.w_out[i]);
                c2.w_out.push_back(p1.w_out[i]);
            }
        }
        newNeurons.push_back(c1);
        newNeurons.push_back(c2);
    }
    neurons.clear();
    neurons = newNeurons;

    // Noise
    std::default_random_engine generator;
    std::normal_distribution<double> dist(0.0, 0.1);
    for (unsigned int i = 0; i < n_neurons; i++) {
        for (unsigned int j = 0; j < n_inputs; j++) {
            neurons[i].w_in[j] += dist(generator);
            //if (neurons[i].w_in[j] > 1.0) neurons[i].w_in[j] = 1.0;
            //if (neurons[i].w_in[j] < -1.0) neurons[i].w_in[j] = -1.0;
        }
        for (unsigned int j = 0; j < n_outputs; j++) {
            neurons[i].w_out[j] += dist(generator);
            //if (neurons[i].w_out[j] > 1.0) neurons[i].w_out[j] = 1.0;
            //if (neurons[i].w_out[j] < -1.0) neurons[i].w_out[j] = -1.0;
        }
    }
}

void Pool::setFitness(unsigned int genome, float fitness) {
    for (unsigned int i = 0; i < genomes[genome].genes.size(); i++) {
        genomes[genome].genes[i]->fitness += fitness;
    }
}
