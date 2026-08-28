//
// Created by bernardo on 8/22/26.
//

#include <cassert>
#include <iostream>
#include "headers/GeneticAlgorithm.h"
#include "headers/GeneFactory.h"
#include "headers/Population.h"

GeneticAlgorithm::GeneticAlgorithm(OptimizationGoal g, CrossoverFunction cf,
                                  std::vector<ConstraintFunction> c,
                                  double pw) {
   goal = g;
   crossover_fn = cf;
   constraints = c;
   penalty_weight = pw;
}

bool GeneticAlgorithm::is_better( const Chromosome& candidate, const Chromosome& current_best) {
   if (goal == OptimizationGoal::Maximize) {
      return candidate.get_fitness() > current_best.get_fitness();
   }
   return candidate.get_fitness() < current_best.get_fitness();

}

Chromosome GeneticAlgorithm::tournament_selection(const std::vector<Chromosome>& population, int tournament_size, std::mt19937& gen) {
   std::vector<Chromosome> random_sample;
   random_sample.reserve(tournament_size);
   std::sample(population.begin(), population.end(), std::back_inserter(random_sample), tournament_size, gen);
   const Chromosome* best = &random_sample[0];
   for (const auto& chromosome : random_sample) {
      if (is_better(chromosome, *best)) { best = &chromosome; }
   }
   return *best;
}

std::pair<Chromosome, Chromosome> GeneticAlgorithm::uniform_crossover(const Chromosome& parent1, const Chromosome& parent2, std::mt19937& gen) {
   std::bernoulli_distribution coin(0.5);
   size_t n = parent1.get_genes().size();

   std::vector<Gene> child1_genes, child2_genes;
   child1_genes.reserve(n);
   child2_genes.reserve(n);

   for (size_t i = 0; i < n; i++) {
      if (coin(gen)) {
         child1_genes.push_back(parent1.get_genes()[i]);
         child2_genes.push_back(parent2.get_genes()[i]);
      } else {
         child1_genes.push_back(parent2.get_genes()[i]);
         child2_genes.push_back(parent1.get_genes()[i]);
      }
   }
   return { Chromosome(child1_genes, 0.0), Chromosome(child2_genes, 0.0) };
}

void GeneticAlgorithm::mutate(Chromosome& chromosome, const std::vector<GeneBounds>& bounds, double mutation_rate, std::mt19937& gen) {
   assert(mutation_rate > 0.0);
   assert(bounds.size() == chromosome.get_genes().size());

   std::bernoulli_distribution dist(mutation_rate);
   for (size_t i = 0; i < bounds.size(); i++) {
      if (dist(gen)) {
         chromosome.set_gene(static_cast<int>(i) ,GeneFactory::generate_random_gene(bounds[i], gen));
      }
   }
}

std::pair<Chromosome, double> GeneticAlgorithm::evolve(int max_generations, int population_size, double mutation_rate, int tournament_size,
                                     std::vector<GeneBounds>& bounds, std::function<double(const std::vector<double>&)> fitness_function) {
   std::random_device rd;
   std::mt19937 gen(rd());
   Population population(population_size, bounds, gen);
   population.evaluate_population(fitness_function);

   for (auto& chromosome : population.population1()) {
      chromosome.set_fitness(adjust_fitness(GeneFactory::chromosome_to_doubles(chromosome.get_genes()),
                                                chromosome.get_fitness()));
   }

   for (int i = 0; i < max_generations; i++) {
      std::vector<Chromosome> pop2;
      pop2.reserve(population_size);
      pop2.push_back(population.get_best(goal).first);
      for (int j = 0; j < (population_size - 1) / 2; j++) {
         Chromosome p1 = tournament_selection(population.population1(), tournament_size, gen);
         Chromosome p2 = tournament_selection(population.population1(), tournament_size, gen);

         std::pair<Chromosome, Chromosome> sons = uniform_crossover(p1, p2, gen);

         mutate(sons.first, bounds, mutation_rate, gen);
         mutate(sons.second, bounds, mutation_rate, gen);

         pop2.push_back(sons.second);
         pop2.push_back(sons.first);
      }
      population.set_population(pop2);
      population.evaluate_population(fitness_function);

      for (auto& chromosome : population.population1()) {
         chromosome.set_fitness(adjust_fitness(GeneFactory::chromosome_to_doubles(chromosome.get_genes()),
                                                   chromosome.get_fitness()));
      }

   }
   return population.get_best(goal);

}
double GeneticAlgorithm::adjust_fitness(const std::vector<double>& values, double fitness) {
   double total_penalty = 0;
   for (const auto& constraint : constraints) {
      double violation = constraint(values);
      if (violation > 0) {
         total_penalty += violation;
      }
   }
   if (goal == OptimizationGoal::Maximize) {
      return fitness - penalty_weight * total_penalty;
   } return fitness + penalty_weight * total_penalty;

}