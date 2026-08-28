//
// Created by bernardo on 8/21/26.
//

#include <random>

#include "headers/Population.h"
#include "headers/GeneticTypes.h"
#include <variant>
#include "headers/GeneFactory.h"
#include "headers/GeneticAlgorithm.h"


std::vector<Chromosome>& Population::population1() {
    return population;
}

void Population::set_population(const std::vector<Chromosome> &p) {
    population = p;
}

Population::Population(const int population_size, const std::vector<GeneBounds> &bounds, std::mt19937& gen) {
    generate_population(population_size, bounds, gen);
}

void Population::generate_population(const int population_size, const std::vector<GeneBounds> &bounds, std::mt19937& gen) {
    population.reserve(population_size);
    for (int i = 0; i < population_size; i++) {
        population.push_back(Chromosome::generate_random_chromosome(bounds, gen));
    }
}

void Population::evaluate_population(std::function<double(const std::vector<double>&)> fitness_function) {

    for ( auto& chromosome : population) {
        std::vector<double> values = GeneFactory::chromosome_to_doubles(chromosome.get_genes());
        chromosome.set_fitness( fitness_function(values) );
    }
}

std::pair<Chromosome,double> Population::get_best(GeneticAlgorithm::OptimizationGoal goal) {
    double best_fitness = population[0].get_fitness();
    Chromosome best_chromosome = population[0];
    if (goal == GeneticAlgorithm::OptimizationGoal::Maximize) {
        for (const auto& chromosome: population) {
            if (chromosome.get_fitness() > best_fitness) {best_fitness = chromosome.get_fitness(); best_chromosome = chromosome; }
        }
        return std::make_pair(best_chromosome, best_fitness);
    }
    for (const auto& chromosome: population) {
        if (chromosome.get_fitness() < best_fitness) {best_fitness = chromosome.get_fitness(); best_chromosome = chromosome; }
    }
    return std::make_pair(best_chromosome, best_fitness);
}


