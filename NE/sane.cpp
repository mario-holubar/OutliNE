#include "sane.h"
#include "QDebug"
#include "time.h"

SANEGene::SANEGene(Neuron n) {
    neuron = n;
}

SANE::SANE() {
    name = "SANE";
}

void SANE::paramDialog(ParamDialog *d) {
    NE::paramDialog(d);
    d->addSpinBox("Number of neurons", &n_neurons, 4, 128);
}

void SANE::init(bool reset) {
    NE::init(reset);
    if (reset) genes.clear();
    // If new n_neurons is smaller, random subpopulations get discarded
    while (genes.size() > n_neurons) genes.pop_back();
    // If new n_neurons is larger, random subpopulations get added
    if (genes.size() < n_neurons) makeGenes();

    //makeGenomes();
}

void SANE::makeGenes() {
    std::normal_distribution<double> dist(0.0, double(initialWeightVariance));
    while (genes.size() < n_neurons) {
        Neuron n;
        for (unsigned i = 0; i < n_inputs; i++) {
            double weight = dist(rand);
            n.w_in[i] = weight;
        }
        for (unsigned i = 0; i < n_outputs; i++) {
            double weight = dist(rand);
            n.w_out[i] = weight;
        }
        genes.push_back(SANEGene(n));
    }
}

void SANE::makeGenomes() {
    genomes = std::vector<SANEGenome>(n_genomes);
    nets = std::vector<NeuralNet>(n_genomes);
    for (unsigned i = 0; i < n_genomes; i++) {
        SANEGenome g;
        NeuralNet n;
        for(unsigned j = 0; j < neuronsPerGenome; j++) {
            unsigned index = unsigned(rand.generate()) % n_neurons;
            genes[index].n_genomes++;
            g.genes.push_back(&(genes[index]));
            n.addNeuron(genes[index].neuron);
        }
        genomes[i] = g;
        nets[i] = n;
    }
}

void SANE::setFitness(unsigned genome, float fitness) {
    for (unsigned i = 0; i < neuronsPerGenome; i++) {
        genomes[genome].genes[i]->fitness += fitness;
    }
}

bool fitnessSort(SANEGene a, SANEGene b) {
    return a.fitness > b.fitness;
}

//TODO delta-coding? Variable topology?
void SANE::newGeneration() {
    // Normalize fitnesses
    for (unsigned i = 0; i < n_neurons; i++) {
        genes[i].fitness /= genes[i].n_genomes;
    }

    std::sort(genes.begin(), genes.end(), fitnessSort);

    // Crossover
    std::vector<SANEGene> newGenes = std::vector<SANEGene>();
    unsigned n;
    for (n = 0; n < n_neurons / 4; n++) { // keep top quarter
        newGenes.push_back(SANEGene(genes[n].neuron));
    }
    Neuron p1, p2;
    for (; n < n_neurons; n++) {
        // Tournament selection for both parents
        float bestFitness = -1.0f;
        unsigned bestIndex = 0;
        for (unsigned k = 0; k < tournamentSize; k++) {
            unsigned c = unsigned(rand.generate()) % n_neurons;
            if (genes[c].fitness > bestFitness) {
                bestFitness = genes[c].fitness;
                bestIndex = c;
            }
        }
        p1 = genes[bestIndex].neuron;
        bestFitness = -1.0f;
        bestIndex = 0;
        for (unsigned k = 0; k < tournamentSize; k++) {
            unsigned c = unsigned(rand.generate()) % n_neurons;
            if (genes[c].fitness > bestFitness) {
                bestFitness = genes[c].fitness;
                bestIndex = c;
            }
        }
        p2 = genes[bestIndex].neuron;

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
        newGenes.push_back(SANEGene(c1));
        //newGenes.push_back(ESPGene(c2));

        // Mutation
        if (mutationNoiseVariance > 0.0f) {
            std::normal_distribution<double> dist(0.0, double(mutationNoiseVariance));
            for (unsigned j = 0; j < n_inputs; j++) {
                newGenes[n].neuron.w_in[j] += dist(rand);
            }
            for (unsigned j = 0; j < n_outputs; j++) {
                newGenes[n].neuron.w_out[j] += dist(rand);
            }
        }
    }
    genes = newGenes;
}
