#include <iostream>
#include <vector>
#include "headers/GeneticAlgorithm.h"
#include "headers/GeneticTypes.h"
#include "headers/GeneFactory.h"


int main() {
    // 1. Gene bounds: x1 real [0,10], x2 real [-5,5], x3 bool
    std::vector<GeneBounds> bounds = {
        std::pair<double, double>{0.0, 10.0},   // x1
        std::pair<double, double>{-5.0, 5.0},   // x2
        std::monostate{}                          // x3 (bool)
    };

    // 2. Restrições (convenção: <= 0 satisfeita, > 0 violada, valor = tamanho da violação)

    // Restrição 1: x1 + 3*x2 <= 5  →  violação = (x1 + 3*x2) - 5
    auto constraint1 = [](const std::vector<double>& x) {
        return (x[0] + 3 * x[1]) - 5.0;
    };

    // Restrição 2: x1 - x2 <= 8  →  violação = (x1 - x2) - 8
    // (só pra mostrar múltiplas restrições ativas ao mesmo tempo)
    auto constraint2 = [](const std::vector<double>& x) {
        return (x[0] - x[1]) - 8.0;
    };

    std::vector<GeneticAlgorithm::ConstraintFunction> constraints = { constraint1, constraint2 };

    // 3. Função objetivo: maximizar x1 + x2 (sem penalidade, o ótimo "livre" seria x1=10, x2=5)
    //    Com a restrição x1 + 3x2 <= 5, o algoritmo deveria encontrar um ponto na fronteira dessa restrição.
    auto objective = [](const std::vector<double>& x) {
        return x[0] + x[1];
    };

    // 4. Configura e roda o GA
    GeneticAlgorithm ga(
        GeneticAlgorithm::OptimizationGoal::Minimize,
        GeneticAlgorithm::uniform_crossover,     // crossover default
        constraints,
        1000.0                  // penalty_weight
    );

    auto result = ga.evolve(200, 30, 0.1, 3, bounds, objective);

    // 5. Imprime resultado
    std::cout << "Melhor solucao encontrada:\n";
    for (const auto& gene : result.first.get_genes()) {
        std::visit([](const auto& v) { std::cout << v << " "; }, gene);
    }
    std::cout << "\nFitness ajustada: " << result.second << "\n";

    // 6. Confere manualmente se a restrição foi respeitada
    auto values = GeneFactory::chromosome_to_doubles(result.first.get_genes());
    double check1 = values[0] + 3 * values[1];
    std::cout << "x1 + 3*x2 = " << check1 << " (deveria ser <= 5)\n";

    return 0;
}