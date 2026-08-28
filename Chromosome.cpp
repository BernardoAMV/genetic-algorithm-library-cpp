//
// Created by bernardo on 8/21/26.
//

#include "headers/GeneticTypes.h"
#include "headers/Chromosome.h"
#include "headers/GeneFactory.h"
#include <vector>
#include <utility>
#include <iostream>
#include <random>

Chromosome::Chromosome() : genes(std::vector<Gene>()) {};

const std::vector<Gene>& Chromosome::get_genes() const {
    return genes;
}

Chromosome::Chromosome(const std::vector<Gene>& g, double f): genes(g), fitness(f) {};

bool Chromosome::set_gene(int index, const Gene& gene) {
    if (index < 0 || index >= genes.size()) {
        return false;
    }
    genes[index] = gene;
    return true;
}

const Gene Chromosome::get_gene(int index) const {
    if (index < 0 || index >= genes.size()) {
        throw std::out_of_range("Index out of range");
    }
    return genes[index];
}

void Chromosome::set_fitness(double f) {
    fitness = f;
}

const double Chromosome::get_fitness() const {
    return fitness;
}

size_t Chromosome::size() const {
    return genes.size();
}

Chromosome Chromosome::generate_random_chromosome(const std::vector<GeneBounds> &bounds, std::mt19937 &gen) {
    std::vector<Gene> genes;
    genes.reserve(bounds.size());
    for (const auto& bound: bounds) {
        genes.push_back(GeneFactory::generate_random_gene(bound, gen));
    }
    return Chromosome(genes, 0.0);

}

