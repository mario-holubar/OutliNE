#include "cosyne.h"
#include "QDebug"
#include "time.h"

CosyneGene::CosyneGene(double w) {
    weight = w;
}

unsigned Cosyne::subpopulationsPerGenome() {
    return neuronsPerGenome * (n_inputs + n_outputs);
}

Cosyne::Cosyne() {
    name = "CoSyNE";
}

void Cosyne::paramDialog(ParamDialog *d) {
    NE::paramDialog(d);
}

void Cosyne::init(bool reset) {
    NE::init(reset);
    if (reset) genes.clear();
    // If new subpopulationsPerGenome is smaller, random subpopulations get discarded
    while (genes.size() > subpopulationsPerGenome()) genes.pop_back();
    // If new subpopulationsPerGenome is larger, random subpopulations get added
    bool add = false;
    while (genes.size() < subpopulationsPerGenome()) {
        genes.push_back(std::vector<CosyneGene>());
        add = true;
    }

    // If new n_genomes is smaller, random weights get discarded
    for (unsigned sp = 0; sp < subpopulationsPerGenome(); sp++) {
        while(genes[sp].size() > n_genomes) genes[sp].pop_back();
    }
    // If new n_genomes is larger, random weights get added (not offspring!)
    if (add || genes[0].size() < n_genomes) makeGenes();

    //makeGenomes();
}

void Cosyne::makeGenes() {
    genes.resize(subpopulationsPerGenome());

    std::normal_distribution<double> dist(0.0, double(initialWeightVariance));
    for (unsigned sp = 0; sp < subpopulationsPerGenome(); sp++) {
        while (genes[sp].size() < n_genomes) {
            double weight = dist(rand);
            genes[sp].push_back(CosyneGene(weight));
        }
    }
}

void Cosyne::makeGenomes() {
    genomes = std::vector<CosyneGenome>(n_genomes);
    nets = std::vector<NeuralNet>(n_genomes);
    for (unsigned i = 0; i < n_genomes; i++) {
        CosyneGenome g;
        for(unsigned j = 0; j < subpopulationsPerGenome(); j++) {
            g.genes.push_back(&(genes[j][i]));
        }
        genomes[i] = g;

        NeuralNet nn;
        unsigned index = 0;
        for(unsigned j = 0; j < neuronsPerGenome; j++) {
            Neuron n;
            for(unsigned k = 0; k < n_inputs; k++) {
                n.w_in[k] = g.genes[index++]->weight;
            }
            for(unsigned k = 0; k < n_outputs; k++) {
                n.w_out[k] = g.genes[index++]->weight;
            }
            nn.addNeuron(n);
        }
        nets[i] = nn;
    }
}

void Cosyne::setFitness(unsigned genome, float fitness) {
    genomes[genome].fitness = fitness;
}

bool fitnessSort(CosyneGenome a, CosyneGenome b) {
    return a.fitness > b.fitness;
}

void Cosyne::newGeneration() {
    std::normal_distribution<double> dist(0.0, double(mutationNoiseVariance));

    std::sort(genomes.begin(), genomes.end(), fitnessSort);

    for (unsigned sp = 0; sp < subpopulationsPerGenome(); sp++) {
        // Crossover
        std::vector<CosyneGene> newGenes;
        unsigned n = 0;
        while (n < n_genomes / 4) newGenes.push_back(*(genomes[n++].genes[sp])); //keep top quarter

        //permute top quarter ( = all except offspring)
        for (unsigned i = n_genomes / 4 - 1; i > 0; i--) {
            unsigned ind = rand() % (i + 1);
            CosyneGene temp = newGenes[i];
            newGenes[i] = newGenes[ind];
            newGenes[ind] = temp;
        }

        while (n < n_genomes) {
            // Tournament selection for both parents
            float bestFitness = -1.0f;
            unsigned bestIndex = 0;
            for (unsigned k = 0; k < tournamentSize; k++) {
                unsigned c = unsigned(rand.generate()) % (n_genomes / 4); //recombine top quarter
                if (genomes[c].fitness > bestFitness) {
                    bestFitness = genomes[c].fitness;
                    bestIndex = c;
                }
            }
            double p1 = genomes[bestIndex].genes[sp]->weight;
            bestFitness = -1.0f;
            bestIndex = 0;
            for (unsigned k = 0; k < tournamentSize; k++) {
                unsigned c = unsigned(rand.generate()) % (n_genomes / 4); //recombine top quarter
                if (genomes[c].fitness > bestFitness) {
                    bestFitness = genomes[c].fitness;
                    bestIndex = c;
                }
            }
            double p2 = genomes[bestIndex].genes[sp]->weight;

            // Random crossover
            double w = rand() % 2 ? p1 : p2;
            w += dist(rand);
            newGenes.push_back(CosyneGene(w));
            n++;
        }
        genes[sp] = newGenes;
    }
}
