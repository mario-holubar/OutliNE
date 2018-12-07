#include "esp.h"
#include "QDebug"
#include "time.h"

ESPParams::ESPParams(unsigned inputs, unsigned outputs) {
    n_inputs = inputs + 0; // 0 bias neurons
    n_outputs = outputs;
}

void ESPParams::paramDialog(ParamDialog *d) {
    d->addSpinBox("Total steps per generation", &tMax, 1, 9999);
    d->addSpinBox("Number of neurons per subpopulation", &neuronsPerSubpopulation, 2, 64);
    d->addSpinBox("Number of individuals", &n_genomes, 16, 512);
    d->addSpinBox("Subpopulations per individual", &subpopulationsPerGenome, 1, 32);
    d->addSpacer();
    d->addDoubleSpinBox("Initial weight variance", &initialWeightVariance, 0.01f, 1.0f);
    d->addDoubleSpinBox("Mutation noise variance", &mutationNoiseVariance, 0.0f, 1.0f);
    d->addDoubleSpinBox("Sigmoid steepness", &sigmoidSteepness, 1.0f, 10.0f);
    d->addSpinBox("Selection pressure (tournament size)", &tournamentSize, 1, 16);
}

ESPGenome::ESPGenome(ESPParams *p) {
    params = p;
}

void ESPNeuralNet::from_genome(ESPGenome g) {
    params = g.params;
    neurons.clear();
    for (unsigned i = 0; i < g.genes.size(); i++) {
        neurons.push_back(g.genes[i]);
    }
}

double ESPNeuralNet::sigmoid(double x) {
    //sigmoid steepness (exp) controls how much outputs can change on small input changes
    //small: smoother but doesn't learn new things as quickly
    return 2.0 / (1.0 + std::exp(-double(params->sigmoidSteepness) * x)) - 1;
}

std::vector<double> ESPNeuralNet::evaluate(std::vector<double> inputs) {
    std::vector<double> outputs(params->n_outputs, 0.0);

    // Bias neurons
    while (inputs.size() < params->n_inputs) inputs.push_back(1.0);

    // Sum inputs
    for (unsigned n = 0; n < neurons.size(); n++) {
        neurons[n]->value = 0.0;
        for (unsigned i = 0; i < params->n_inputs; i++) {
            neurons[n]->value += inputs[i] * neurons[n]->w_in[i];
        }
    }
    // Neuron activation function
    for (unsigned n = 0; n < neurons.size(); n++) {
        neurons[n]->value = sigmoid(neurons[n]->value);
    }
    // Sum outputs
    for (unsigned n = 0; n < neurons.size(); n++) {
        for (unsigned i = 0; i < params->n_outputs; i++) {
            outputs[i] += neurons[n]->value * neurons[n]->w_out[i];
        }
    }
    // Output activation function
    for (unsigned i = 0; i < params->n_outputs; i++) {
        outputs[i] = sigmoid(outputs[i]);
    }

    return outputs;
}

ESPPool::ESPPool() {

}

// Generate random neurons
void ESPPool::makeNeurons(bool reset) {
    if (reset) {
        for (unsigned sp = 0; sp < params->subpopulationsPerGenome; sp++) neurons[sp]->clear();
    }
    std::random_device rd;
    std::mt19937 generator {rd()};
    std::normal_distribution<double> dist(0.0, double(params->initialWeightVariance));
    for (unsigned sp = 0; sp < params->subpopulationsPerGenome; sp++) {
        while (neurons[sp]->size() < params->neuronsPerSubpopulation) {
            ESPNeuron n;
            for (unsigned i = 0; i < params->n_inputs; i++) {
                double weight = dist(generator);
                n.w_in.push_back(weight);
            }
            for (unsigned i = 0; i < params->n_outputs; i++) {
                double weight = dist(generator);
                n.w_out.push_back(weight);
            }
            neurons[sp]->push_back(n);
        }
    }
}

void ESPPool::init(ESPParams *p) {
    params = p;

    rand.seed(rand.generate()); // dunno if this is ok

    // If new subpopulationsPerGenome is smaller, random subpopulations get discarded
    while(neurons.size() > params->subpopulationsPerGenome) neurons.pop_back();
    // If new subpopulationsPerGenome is larger, random subpopulations get added
    bool add = false;
    while (neurons.size() < params->subpopulationsPerGenome) {
        neurons.push_back(new std::vector<ESPNeuron>());
        add = true;
    }
    if (add) makeNeurons(false);

    // If new neuronsPerSubpopulation is smaller, random neurons get discarded
    for (unsigned sp = 0; sp < params->subpopulationsPerGenome; sp++) {
        while(neurons[sp]->size() > params->neuronsPerSubpopulation) neurons[sp]->pop_back();
    }
    // If new neuronsPerSubpopulation is larger, random neurons get added (not offspring!)
    if (neurons[0]->size() < params->neuronsPerSubpopulation) makeNeurons(false);

    //makeGenomes();
}

void ESPPool::makeGenomes() {
    genomes.clear();
    for (unsigned i = 0; i < params->n_genomes; i++) {
        ESPGenome g(params);
        for(unsigned j = 0; j < params->subpopulationsPerGenome; j++) {
            unsigned index = unsigned(rand.generate()) % params->neuronsPerSubpopulation;
            neurons[j]->data()[index].n_genomes++;
            g.genes.push_back(&(neurons[j]->data()[index]));
        }
        genomes.push_back(g);
    }
}

//TODO delta-coding? Variable topology?
void ESPPool::newGeneration() {
    for (unsigned sp = 0; sp < params->subpopulationsPerGenome; sp++) {
        // Normalize fitnesses
        for (unsigned i = 0; i < params->neuronsPerSubpopulation; i++) {
            neurons[sp]->data()[i].fitness /= neurons[sp]->data()[i].n_genomes;
        }

        // Crossover
        std::vector<ESPNeuron> *newNeurons = new std::vector<ESPNeuron>;
        ESPNeuron p1, p2;
        for (unsigned n = 0; n < params->neuronsPerSubpopulation / 2; n++) {
            // Tournament selection for both parents
            float bestFitness = -1.0f;
            unsigned bestIndex = 0;
            for (unsigned k = 0; k < params->tournamentSize; k++) {
                unsigned c = unsigned(rand.generate()) % params->neuronsPerSubpopulation;
                if (neurons[sp]->data()[c].fitness > bestFitness) {
                    bestFitness = neurons[sp]->data()[c].fitness;
                    bestIndex = c;
                }
            }
            p1 = neurons[sp]->data()[bestIndex];
            bestFitness = -1.0f;
            bestIndex = 0;
            for (unsigned k = 0; k < params->tournamentSize; k++) {
                unsigned c = unsigned(rand.generate()) % params->neuronsPerSubpopulation;
                if (neurons[sp]->data()[c].fitness > bestFitness) {
                    bestFitness = neurons[sp]->data()[c].fitness;
                    bestIndex = c;
                }
            }
            p2 = neurons[sp]->data()[bestIndex];

            // Random crossover
            ESPNeuron c1, c2;
            for (unsigned i = 0; i < params->n_inputs; i++) {
                if (rand.generate() % 2) {
                    c1.w_in.push_back(p1.w_in[i]);
                    c2.w_in.push_back(p2.w_in[i]);
                }
                else {
                    c1.w_in.push_back(p2.w_in[i]);
                    c2.w_in.push_back(p1.w_in[i]);
                }
            }
            for (unsigned i = 0; i < params->n_outputs; i++) {
                if (rand.generate() % 2) {
                    c1.w_out.push_back(p1.w_out[i]);
                    c2.w_out.push_back(p2.w_out[i]);
                }
                else {
                    c1.w_out.push_back(p2.w_out[i]);
                    c2.w_out.push_back(p1.w_out[i]);
                }
            }
            newNeurons->push_back(c1);
            newNeurons->push_back(c2);
        }
        delete neurons[sp];
        neurons[sp] = newNeurons;

        // Noise
        if (params->mutationNoiseVariance > 0.0f) {
            std::default_random_engine generator;
            std::normal_distribution<double> dist(0.0, double(params->mutationNoiseVariance));
            for (unsigned i = 0; i < params->neuronsPerSubpopulation; i++) {
                for (unsigned j = 0; j < params->n_inputs; j++) {
                    neurons[sp]->data()[i].w_in[j] += dist(generator);
                    //if (neurons[sp]->data()[i].w_in[j] > 1.0) neurons[sp]->data()[i].w_in[j] = 1.0;
                    //if (neurons[sp]->data()[i].w_in[j] < -1.0) neurons[sp]->data()[i].w_in[j] = -1.0;
                }
                for (unsigned j = 0; j < params->n_outputs; j++) {
                    neurons[sp]->data()[i].w_out[j] += dist(generator);
                    //if (neurons[sp]->data()[i].w_out[j] > 1.0) neurons[sp]->data()[i].w_out[j] = 1.0;
                    //if (neurons[sp]->data()[i].w_out[j] < -1.0) neurons[sp]->data()[i].w_out[j] = -1.0;
                }
            }
        }
    }
}

void ESPPool::setFitness(unsigned genome, float fitness) {
    for (unsigned i = 0; i < genomes[genome].genes.size(); i++) {
        genomes[genome].genes[i]->fitness += fitness;
    }
}
