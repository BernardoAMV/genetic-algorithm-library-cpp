//
// Created by bernardo on 8/21/26.
//

#include "headers/GeneFactory.h"
#include "headers/GeneticTypes.h"

Gene GeneFactory::generate_random_gene(const GeneBounds &bounds, std::mt19937& gen) {
    return std::visit([&gen] (const auto & arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr(std::is_same_v<T, std::pair<int, int>>) {
            std::uniform_int_distribution<int> dist(arg.first, arg.second);
            return Gene(dist(gen));
        } else if constexpr(std::is_same_v<T, std::pair<double, double>>) {
            std::uniform_real_distribution<double> dist(arg.first, arg.second);
            return Gene(dist(gen));
        } else if constexpr(std::is_same_v<T, std::monostate>) {
            std::bernoulli_distribution dist (0.5);
            return Gene(dist(gen));
        }
    }, bounds );
}

double GeneFactory::gene_to_double(const Gene& gene) {
    return std::visit([](const auto& v) -> double {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) {
            return v ? 1.0 : 0.0;
        } else {
            return static_cast<double>(v);
        }
    }, gene);
}

std::vector<double> GeneFactory::chromosome_to_doubles(const std::vector<Gene>& genes) {
    std::vector<double> result;
    result.reserve(genes.size());
    for (const auto& gene : genes) {
        result.push_back(gene_to_double(gene));
    }
    return result;
}