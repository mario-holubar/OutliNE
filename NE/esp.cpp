#include "esp.h"
#include "QDebug"
#include "time.h"

ESPGene::ESPGene(Neuron n) {
    neuron = n;
}

ESP::ESP() {
    name = "ESP";
}

void ESP::paramDialog(ParamDialog *d) {
    NE::paramDialog(d);
    d->addSpinBox("Number of neurons per subpopulation", &neuronsPerSubpopulation, 2, 32);
    d->addSpinBox("Generations of stagnation before burst mutation", &burstMutationStagnate, 2, 32);
}

void ESP::init(bool reset) {
    NE::init(reset);
    if (reset) genes.clear();
    // If new subpopulationsPerGenome is smaller, random subpopulations get discarded
    while (genes.size() > neuronsPerGenome) genes.pop_back();
    // If new subpopulationsPerGenome is larger, random subpopulations get added
    bool add = false;
    while (genes.size() < neuronsPerGenome) {
        genes.push_back(std::vector<ESPGene>());//TODO unneccessary?
        add = true;
    }

    // If new neuronsPerSubpopulation is smaller, random neurons get discarded
    for (unsigned sp = 0; sp < neuronsPerGenome; sp++) {
        while(genes[sp].size() > neuronsPerSubpopulation) genes[sp].pop_back();
    }
    // If new neuronsPerSubpopulation is larger, random neurons get added (not offspring!)
    if (add || genes[0].size() < neuronsPerSubpopulation) makeGenes();

    //makeGenomes();
}

void ESP::makeGenes() {
    //genes = std::vector<std::vector<ESPGene>>(subpopulationsPerGenome);
    genes.resize(neuronsPerGenome);

    std::normal_distribution<double> dist(0.0, double(initialWeightVariance));
    for (unsigned sp = 0; sp < neuronsPerGenome; sp++) {
        while (genes[sp].size() < neuronsPerSubpopulation) {
            Neuron n;
            for (unsigned i = 0; i < n_inputs; i++) {
                double weight = dist(rand);
                n.w_in[i] = weight;
            }
            for (unsigned i = 0; i < n_outputs; i++) {
                double weight = dist(rand);
                n.w_out[i] = weight;
            }
            genes[sp].push_back(ESPGene(n));
        }
    }
}

void ESP::makeGenomes() {
    genomes = std::vector<ESPGenome>(n_genomes);
    nets = std::vector<NeuralNet>(n_genomes);
    for (unsigned i = 0; i < n_genomes; i++) {
        ESPGenome g;
        NeuralNet n;
        for(unsigned j = 0; j < neuronsPerGenome; j++) {
            unsigned index = unsigned(rand.generate()) % neuronsPerSubpopulation;
            genes[j][index].n_genomes++;
            g.genes.push_back(&(genes[j][index]));
            n.addNeuron(genes[j][index].neuron);
        }
        genomes[i] = g;
        nets[i] = n;
    }
}

void ESP::setFitness(unsigned genome, float fitness) {
    if (fitness > bestFitness) {
        bestFitness = fitness;
        stagnate = 0;
    }
    for (unsigned i = 0; i < neuronsPerGenome; i++) {
        genomes[genome].genes[i]->fitness += fitness;
    }
}

//TODO delta-coding? Variable topology?
void ESP::newGeneration() {
    for (unsigned sp = 0; sp < neuronsPerGenome; sp++) {
        // Normalize fitnesses
        for (unsigned i = 0; i < neuronsPerSubpopulation; i++) {
            genes[sp][i].fitness /= genes[sp][i].n_genomes;
        }

        // Crossover
        std::vector<ESPGene> newGenes = std::vector<ESPGene>();
        Neuron p1, p2;
        for (unsigned n = 0; n < neuronsPerSubpopulation; n++) {
            // Tournament selection for both parents
            float bestFitness = -1.0f;
            unsigned bestIndex = 0;
            for (unsigned k = 0; k < tournamentSize; k++) {
                unsigned c = unsigned(rand.generate()) % neuronsPerSubpopulation;
                if (genes[sp][c].fitness > bestFitness) {
                    bestFitness = genes[sp][c].fitness;
                    bestIndex = c;
                }
            }
            p1 = genes[sp][bestIndex].neuron;
            bestFitness = -1.0f;
            bestIndex = 0;
            for (unsigned k = 0; k < tournamentSize; k++) {
                unsigned c = unsigned(rand.generate()) % neuronsPerSubpopulation;
                if (genes[sp][c].fitness > bestFitness) {
                    bestFitness = genes[sp][c].fitness;
                    bestIndex = c;
                }
            }
            p2 = genes[sp][bestIndex].neuron;

            // Random crossover
            //Neuron c1, c2;
            Neuron c1;
            for (unsigned i = 0; i < n_inputs; i++) {
                if (rand.generate() % 2) {
                    c1.w_in[i] = p1.w_in[i];
                    //c2.w_in[i] = p2.w_in[i];
                }
                else {
                    c1.w_in[i] = p2.w_in[i];
                    //c2.w_in[i] = p1.w_in[i];
                }
            }
            for (unsigned i = 0; i < n_outputs; i++) {
                if (rand.generate() % 2) {
                    c1.w_out[i] = p1.w_out[i];
                    //c2.w_out[i] = p2.w_out[i];
                }
                else {
                    c1.w_out[i] = p2.w_out[i];
                    //c2.w_out[i] = p1.w_out[i];
                }
            }
            newGenes.push_back(ESPGene(c1));
            //newGenes.push_back(ESPGene(c2));
        }
        genes[sp] = newGenes;

        // Noise
        // TODO move inside top loop
        if (mutationNoiseVariance > 0.0f) {
            std::normal_distribution<double> dist(0.0, double(mutationNoiseVariance));
            for (unsigned i = 0; i < neuronsPerSubpopulation; i++) {
                for (unsigned j = 0; j < n_inputs; j++) {
                    genes[sp][i].neuron.w_in[j] += dist(rand);
                    //if (neurons[sp]->data()[i].w_in[j] > 1.0) neurons[sp]->data()[i].w_in[j] = 1.0;
                    //if (neurons[sp]->data()[i].w_in[j] < -1.0) neurons[sp]->data()[i].w_in[j] = -1.0;
                }
                for (unsigned j = 0; j < n_outputs; j++) {
                    genes[sp][i].neuron.w_out[j] += dist(rand);
                    //if (neurons[sp]->data()[i].w_out[j] > 1.0) neurons[sp]->data()[i].w_out[j] = 1.0;
                    //if (neurons[sp]->data()[i].w_out[j] < -1.0) neurons[sp]->data()[i].w_out[j] = -1.0;
                }
            }
        }
    }

    stagnate++;
    //qDebug() << stagnate;
    float burstMutationVariance = 0.05f;
    if (stagnate > burstMutationStagnate) {
        // Burst mutation
        stagnate = 0;
        bestFitness = 0.0f;
        std::default_random_engine generator;
        std::cauchy_distribution<double> dist(0.0, double(burstMutationVariance));
        qDebug() << "burst mutation at gen " + QString::number(gen);
        for (unsigned sp = 0; sp < neuronsPerGenome; sp++) {
            for (unsigned i = 0; i < neuronsPerSubpopulation; i++) {
                //qDebug() << dist(rand);
                for (unsigned j = 0; j < n_inputs; j++) {
                    genes[sp][i].neuron.w_in[j] += dist(rand);
                    //if (neurons[sp]->data()[i].w_in[j] > 1.0) neurons[sp]->data()[i].w_in[j] = 1.0;
                    //if (neurons[sp]->data()[i].w_in[j] < -1.0) neurons[sp]->data()[i].w_in[j] = -1.0;
                }
                for (unsigned j = 0; j < n_outputs; j++) {
                    genes[sp][i].neuron.w_out[j] += dist(rand);
                    //if (neurons[sp]->data()[i].w_out[j] > 1.0) neurons[sp]->data()[i].w_out[j] = 1.0;
                    //if (neurons[sp]->data()[i].w_out[j] < -1.0) neurons[sp]->data()[i].w_out[j] = -1.0;
                }
            }
        }
    }

    //makeGenomes();
}
