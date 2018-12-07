#include "sane.h"
#include "QDebug"
#include "time.h"

SANEParams::SANEParams(unsigned inputs, unsigned outputs) {
    n_inputs = inputs + 0; // 0 bias neurons
    n_outputs = outputs;
}

void SANEParams::paramDialog(ParamDialog *d) {
    d->addSpinBox("Total steps per generation", &tMax, 1, 9999);
    d->addSpinBox("Number of neurons", &n_neurons, 8, 128);
    d->addSpinBox("Number of individuals", &n_genomes, 16, 512);
    d->addSpinBox("Neurons per individual", &neuronsPerGenome, 1, 32);
    d->addSpacer();
    d->addDoubleSpinBox("Initial weight variance", &initialWeightVariance, 0.01f, 1.0f);
    d->addDoubleSpinBox("Mutation noise variance", &mutationNoiseVariance, 0.0f, 1.0f);
    d->addDoubleSpinBox("Sigmoid steepness", &sigmoidSteepness, 1.0f, 10.0f);
    d->addSpinBox("Selection pressure (tournament size)", &tournamentSize, 1, 16);
}

SANEGenome::SANEGenome(SANEParams *p) {
    params = p;
}

void SANENeuralNet::from_genome(SANEGenome g) {
    params = g.params;
    neurons.clear();
    for (unsigned i = 0; i < g.genes.size(); i++) {
        neurons.push_back(g.genes[i]);
    }
}

double SANENeuralNet::sigmoid(double x) {
    //sigmoid steepness (exp) controls how much outputs can change on small input changes
    //small: smoother but doesn't learn new things as quickly
    return 2.0 / (1.0 + std::exp(-double(params->sigmoidSteepness) * x)) - 1;
}

std::vector<double> SANENeuralNet::evaluate(std::vector<double> inputs) {
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

SANEPool::SANEPool() {

}

// Generate random neurons
void SANEPool::makeNeurons(bool reset) {
    if (reset) neurons.clear();
    std::random_device rd;
    std::mt19937 generator {rd()};
    std::normal_distribution<double> dist(0.0, double(params->initialWeightVariance));
    while (neurons.size() < params->n_neurons) {
        SANENeuron n;
        for (unsigned i = 0; i < params->n_inputs; i++) {
            double weight = dist(generator);
            n.w_in.push_back(weight);
        }
        for (unsigned i = 0; i < params->n_outputs; i++) {
            double weight = dist(generator);
            n.w_out.push_back(weight);
        }
        neurons.push_back(n);
    }
}

void SANEPool::init(SANEParams *p) {
    params = p;

    rand.seed(rand.generate()); // dunno if this is ok

    // If new n_neurons is smaller, random neurons get discarded
    while(neurons.size() > params->n_neurons) neurons.pop_back();
    // If new n_neurons is larger, random neurons get added (not offspring!)
    if (neurons.size() < params->n_neurons) makeNeurons(false);

    makeGenomes();
}

void SANEPool::makeGenomes() {
    genomes.clear();
    for (unsigned i = 0; i < params->n_genomes; i++) {
        SANEGenome g(params);
        for(unsigned j = 0; j < params->neuronsPerGenome; j++) {
            unsigned index = unsigned(rand.generate()) % params->n_neurons;
            neurons[index].n_genomes++;
            g.genes.push_back(&neurons[index]);
        }
        genomes.push_back(g);
    }
}

void SANEPool::newGeneration() {
    // Normalize fitnesses
    for (unsigned i = 0; i < params->n_neurons; i++) {
        neurons[i].fitness /= neurons[i].n_genomes;
    }

    // Crossover
    std::vector<SANENeuron> newNeurons;
    SANENeuron p1, p2;
    for (unsigned n = 0; n < params->n_neurons / 2; n++) {
        // Tournament selection for both parents
        float bestFitness = -1.0f;
        unsigned bestIndex = 0;
        for (unsigned k = 0; k < params->tournamentSize; k++) {
            unsigned c = unsigned(rand.generate()) % params->n_neurons;
            if (neurons[c].fitness > bestFitness) {
                bestFitness = neurons[c].fitness;
                bestIndex = c;
            }
        }
        p1 = neurons[bestIndex];
        bestFitness = -1.0f;
        bestIndex = 0;
        for (unsigned k = 0; k < params->tournamentSize; k++) {
            unsigned c = unsigned(rand.generate()) % params->n_neurons;
            if (neurons[c].fitness > bestFitness) {
                bestFitness = neurons[c].fitness;
                bestIndex = c;
            }
        }
        p2 = neurons[bestIndex];

        // Random crossover
        SANENeuron c1, c2;
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
        newNeurons.push_back(c1);
        newNeurons.push_back(c2);
    }
    neurons.clear();
    neurons = newNeurons;

    // Noise
    if (params->mutationNoiseVariance > 0.0f) {
        std::default_random_engine generator;
        std::normal_distribution<double> dist(0.0, double(params->mutationNoiseVariance));
        for (unsigned i = 0; i < params->n_neurons; i++) {
            for (unsigned j = 0; j < params->n_inputs; j++) {
                neurons[i].w_in[j] += dist(generator);
                //if (neurons[i].w_in[j] > 1.0) neurons[i].w_in[j] = 1.0;
                //if (neurons[i].w_in[j] < -1.0) neurons[i].w_in[j] = -1.0;
            }
            for (unsigned j = 0; j < params->n_outputs; j++) {
                neurons[i].w_out[j] += dist(generator);
                //if (neurons[i].w_out[j] > 1.0) neurons[i].w_out[j] = 1.0;
                //if (neurons[i].w_out[j] < -1.0) neurons[i].w_out[j] = -1.0;
            }
        }
    }
}

void SANEPool::setFitness(unsigned genome, float fitness) {
    for (unsigned i = 0; i < genomes[genome].genes.size(); i++) {
        genomes[genome].genes[i]->fitness += fitness;
    }
}
