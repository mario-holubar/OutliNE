#include "ne.h"
#include "QDebug"

Neuron::Neuron() {
    w_in = std::vector<double>(NE::n_inputs);
    w_out = std::vector<double>(NE::n_outputs);
}

void NeuralNet::addNeuron(Neuron n) {
    neurons.push_back(n);
}

Neuron *NeuralNet::getNeuron(unsigned neuron) {
    if (neuron >= neurons.size()) {
        qDebug() << "Neural net vector out of range:" << neuron << "( size" << neurons.size() << ").";
        return nullptr;
    }
    return &(neurons[neuron]);
}

unsigned NeuralNet::numberOfNeurons() {
    return unsigned(neurons.size());
}

double NeuralNet::sigmoid(double x) {
    //sigmoid steepness controls how much outputs can change on small input changes
    //small: smoother but doesn't learn new things as quickly
    return 2.0 / (1.0 + std::exp(-double(NE::sigmoidSteepness) * x)) - 1;
}

std::vector<double> NeuralNet::evaluate(std::vector<double> inputs) {
    std::vector<double> outputs(NE::n_outputs, 0.0);

    // Bias neurons
    while (inputs.size() < NE::n_inputs) inputs.push_back(1.0);

    // Sum inputs
    for (unsigned n = 0; n < neurons.size(); n++) {
        neurons[n].value = 0.0;
        for (unsigned i = 0; i < NE::n_inputs; i++) {
            neurons[n].value += inputs[i] * neurons[n].w_in[i];
        }
    }
    // Neuron activation function
    for (unsigned n = 0; n < neurons.size(); n++) {
        neurons[n].value = sigmoid(neurons[n].value);
    }
    // Sum outputs
    for (unsigned n = 0; n < neurons.size(); n++) {
        for (unsigned i = 0; i < NE::n_outputs; i++) {
            outputs[i] += neurons[n].value * neurons[n].w_out[i];
        }
    }
    // Output activation function
    for (unsigned i = 0; i < NE::n_outputs; i++) {
        outputs[i] = sigmoid(outputs[i]);
    }

    return outputs;
}

unsigned NE::n_genomes = 64;
float NE::sigmoidSteepness = 4.0f;
unsigned NE::n_inputs = 0;
unsigned NE::n_outputs = 0;
float NE::initialWeightVariance = 0.5f;
float NE::mutationNoiseVariance = 0.1f;
unsigned NE::tournamentSize = 2; //selection pressure

NE::NE() {
    nets = std::vector<NeuralNet>(n_genomes);
    rand.seed(rand.generate()); // dunno if this is ok
}

NE::~NE() {

}

NeuralNet *NE::getNet(unsigned net) {
    if (net >= nets.size()) {
        qDebug() << "Neural net vector out of range:" << net << "( size" << nets.size() << ").";
        return nullptr;
    }
    return &(nets[net]);
}

void NE::paramDialog(ParamDialog *d) {
    d->addSpinBox("Number of individuals", &n_genomes, 16, 512);
    d->addDoubleSpinBox("Sigmoid steepness", &sigmoidSteepness, 1.0f, 10.0f);
    d->addDoubleSpinBox("Initial weight variance", &initialWeightVariance, 0.01f, 1.0f);
    d->addDoubleSpinBox("Mutation noise variance", &mutationNoiseVariance, 0.0f, 1.0f);
    d->addSpinBox("Selection pressure (tournament size)", &tournamentSize, 1, 16);
    d->addSpacer();
}
