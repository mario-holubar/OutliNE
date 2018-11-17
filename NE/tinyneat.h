#ifndef TINYNEAT_H
#define TINYNEAT_H

/* custom defines:
 * INCLUDE_ENABLED_GENES_IF_POSSIBLE  - if during experiment you found that too many genes are
 *                                      disabled, you can use this option.
 * ALLOW_RECURRENCY_IN_NETWORK	      - allowing recurrent links 
 *
 * GIVING_NAMES_FOR_SPECIES           - giving species unique names (need a dictionary with 
 *                                      names in a file "specie_names.dict"
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <random>
#include <map>
#include <algorithm>
#include <list>
#include <string>

namespace neat {

    typedef struct {
		double connection_mutate_chance = 0.25;
		double perturb_chance = 0.90;
		double crossover_chance = 0.75;
		double link_mutation_chance = 2.0;
		double node_mutation_chance = 0.50;
		double bias_mutation_chance = 0.40;
		double step_size = 0.1;
		double disable_mutation_chance = 0.4;
        double enable_mutation_chance = 0.2;

		void read(std::ifstream& o);
		void write(std::ofstream& o, std::string prefix);
	} mutation_rate_container;

	typedef struct {
        unsigned int population = 240;
		double delta_disjoint = 2.0;
		double delta_weights = 0.4;
		double delta_threshold = 1.3;
		unsigned int stale_species = 15;

		void read(std::ifstream& o);
		void write(std::ofstream& o, std::string prefix);
	} speciating_parameter_container;

	typedef struct {
		unsigned int input_size;
		unsigned int bias_size;
		unsigned int output_size;
		unsigned int functional_nodes;		
		bool recurrent;
	} network_info_container;

	typedef struct {	
        unsigned int innovation_num = unsigned(-1);
        unsigned int from_node = unsigned(-1);
        unsigned int to_node = unsigned(-1);
		double weight = 0.0;
		bool enabled = true;
	} gene;

	class genome {
	private:
        genome();

	public:
		unsigned int fitness = 0;
		unsigned int adjusted_fitness = 0;
		unsigned int global_rank = 0;
		unsigned int max_neuron;
		unsigned int can_be_recurrent = false;

		mutation_rate_container mutation_rates;
		network_info_container network_info;

		std::map<unsigned int, gene> genes;

        genome(network_info_container& info, mutation_rate_container& rates);
		
		genome(const genome&) = default;
	};


	/* a specie is group of genomes which differences is smaller than some threshold */
	typedef struct {
		unsigned int top_fitness = 0;
		unsigned int average_fitness = 0;
		unsigned int staleness = 0;

	#ifdef GIVING_NAMES_FOR_SPECIES
		std::string name;
	#endif
		std::vector<genome> genomes;
	} specie;	

	class innovation_container {
	private:
		unsigned int _number;
		std::map<std::pair<unsigned int, unsigned int>, unsigned int> track;
        void set_innovation_number(unsigned int num);
		friend class pool;
	public:
		innovation_container():_number(0){}			
        void reset();
        unsigned int add_gene(gene& g);
        unsigned int number();
	};


	/* a small world, where individuals (genomes) are making babies and evolving,
	 * becoming better and better after each generation :)
	 */
	class pool {
	private:
        pool();

		/* important part, only accecible for friend */
		innovation_container innovation;

		/* innovation tracking in current generation, should be cleared after each generation */
		std::map<std::pair<unsigned int, unsigned int>, unsigned int> track;


		unsigned int generation_number = 1;
		
		/* evolutionary methods */
		genome crossover(const genome& g1, const genome& g2);
		void mutate_weight(genome& g);
		void mutate_enable_disable(genome& g, bool enable);
		void mutate_link(genome& g, bool force_bias);
		void mutate_node(genome& g);		
		void mutate(genome& g);		

		double disjoint(const genome& g1, const genome& g2);
		double weights(const genome& g1, const genome& g2);
		bool is_same_species(const genome& g1, const genome& g2);

		/* specie ranking */
		void rank_globally();
		void calculate_average_fitness(specie& s);
		unsigned int total_average_fitness();

		/* evolution */
		void cull_species(bool cut_to_one);
		genome breed_child(specie& s);
		void remove_stale_species();
		void remove_weak_species();
		void add_to_species(genome& child);


	public:
		/* pool parameters */
		unsigned int max_fitness = 0;

		/* mutation parameters */
		mutation_rate_container mutation_rates;

		/* species parameters */
		speciating_parameter_container speciating_parameters;

		/* neural network parameters */
		network_info_container network_info;		

		// pool's local random number generator
        std::random_device rd;
		std::mt19937 generator;

		/* species */
		std::list<specie> species;

		// constructor
		pool(unsigned int input, unsigned int output, unsigned int bias = 1, 
                bool rec = false);

		/* next generation */
		void new_generation();
        unsigned int generation();

		/* calculate fitness */
        std::vector<std::pair<specie*, genome*>> get_genomes();

		/* import and export */
		void import_fromfile(std::string filename);
		void export_tofile(std::string filename);		
    };

} // end of namespace neat

#endif
