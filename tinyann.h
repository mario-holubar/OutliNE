#ifndef ARTIFICIAL_NEURAL_NETWORK_H
#define ARTIFICIAL_NEURAL_NETWORK_H

#include <unordered_map>
#include <cmath>
#include <array>
#include <stack>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include "tinyneat.h"

namespace ann {

	enum type {
		RECURRENT,
		NON_RECURRENT
	};	

	class neuron {
	public:		
		int type = 0; // 0 = ordinal, 1 = input, 2 = output, 3 = bias
		double value = 0.0;
		bool visited = false;
		std::vector<std::pair<size_t, double>> in_nodes;
        neuron();
        ~neuron();
	};

	class neuralnet {
	private:
		std::vector<neuron> nodes;
		bool recurrent = false;

		std::vector<size_t> input_nodes;
		std::vector<size_t> bias_nodes;
		std::vector<size_t> output_nodes;

        double sigmoid(double x);

        void evaluate_nonrecurrent(const std::vector<double>& input, std::vector<double>& output);

        void evaluate_recurrent(const std::vector<double>& input, std::vector<double>& output);
	

	public:
		neuralnet(){}

        void from_genome(const neat::genome& a);

        void evaluate(const std::vector<double>& input, std::vector<double>& output);
	
        void import_fromfile(std::string filename);

        void export_tofile(std::string filename);

	};

} // end of namespace ann

#endif
