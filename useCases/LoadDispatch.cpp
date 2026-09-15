// ============================================================================
// Economic Load Dispatch (ELD) - Sistema IEEE de 15 Unidades
// ============================================================================
//
// Problema: minimizar o custo total de combustivel de 15 usinas
// termoeletricas, considerando:
//   - Efeito de ponto de valvula (valve-point effect) na funcao de custo
//   - Restricao de balanco de carga (igualdade): soma das potencias = 2630 MW
//   - Zonas proibidas de operacao (POZ) nas unidades 2, 5, 6 e 12
//
// Fonte dos coeficientes: Al-Roomi, A. R. (2016). Economic Load Dispatch
// Test Systems Repository. Dalhousie University.
// ============================================================================

#include <iostream>
#include <vector>
#include <cmath>

#include "../include/genetic-algorithm-library-cpp/GeneticAlgorithm.h"
#include "../include/genetic-algorithm-library-cpp/GeneticTypes.h"
#include "../include/genetic-algorithm-library-cpp/GeneFactory.h"

// ----------------------------------------------------------------------------
// Coeficientes do sistema (globais, para reuso entre bounds e funcoes)
// ----------------------------------------------------------------------------
static const int NUM_UNITS = 15;
static const double LOAD_DEMAND = 2630.0;

static const double PMIN[NUM_UNITS] = {150, 150, 20, 20, 150, 135, 135, 60,
                                         25, 25, 20, 20, 25, 15, 15};
static const double PMAX[NUM_UNITS] = {455, 455, 130, 130, 470, 460, 465, 300,
                                         162, 160, 80, 80, 85, 55, 55};

static const double A[NUM_UNITS] = {671, 574, 374, 374, 461, 630, 548, 227,
                                      173, 175, 186, 230, 225, 309, 323};
static const double B[NUM_UNITS] = {10.1, 10.2, 8.8, 8.8, 10.4, 10.1, 9.8, 11.2,
                                      11.2, 10.7, 10.2, 9.9, 13.1, 12.1, 12.4};
static const double C[NUM_UNITS] = {0.000299, 0.000183, 0.001126, 0.001126, 0.000205,
                                      0.000301, 0.000364, 0.000338, 0.000807, 0.001203,
                                      0.003586, 0.005513, 0.000371, 0.001929, 0.004447};
static const double D[NUM_UNITS] = {100, 100, 100, 150, 120, 100, 200, 200,
                                      200, 200, 200, 200, 300, 300, 300};
static const double E[NUM_UNITS] = {0.084, 0.084, 0.084, 0.063, 0.077, 0.084, 0.042, 0.042,
                                      0.042, 0.042, 0.042, 0.042, 0.035, 0.035, 0.035};

// ----------------------------------------------------------------------------
// Funcao objetivo: custo total de combustivel, com efeito de ponto de valvula
// ----------------------------------------------------------------------------
double eld_cost(const std::vector<double>& P) {
    double total_cost = 0.0;
    for (int i = 0; i < NUM_UNITS; i++) {
        double valve_effect = std::abs(D[i] * std::sin(E[i] * (PMIN[i] - P[i])));
        total_cost += A[i] + B[i] * P[i] + C[i] * P[i] * P[i] + valve_effect;
    }
    return total_cost;
}

// ----------------------------------------------------------------------------
// Restricao de balanco de carga (igualdade): soma das potencias = LOAD_DEMAND
// ----------------------------------------------------------------------------
double constraint_load_balance(const std::vector<double>& P) {
    double sum = 0.0;
    for (int i = 0; i < NUM_UNITS; i++) {
        sum += P[i];
    }
    return std::abs(sum - LOAD_DEMAND);
}

// ----------------------------------------------------------------------------
// Zonas proibidas de operacao (POZ)
// ----------------------------------------------------------------------------
// Verifica se P caiu dentro de alguma zona proibida; devolve a distancia
// ate a borda mais proxima (violacao), ou -1.0 se estiver fora de todas.
double poz_violation(double P, const std::vector<std::pair<double, double>>& zones) {
    for (const auto& zone : zones) {
        double lower = zone.first, upper = zone.second;
        if (P > lower && P < upper) {
            return std::min(P - lower, upper - P);
        }
    }
    return -1.0;
}

double constraint_poz_unit2(const std::vector<double>& P) {
    return poz_violation(P[1], {{185, 225}, {305, 335}, {420, 450}});
}

double constraint_poz_unit5(const std::vector<double>& P) {
    return poz_violation(P[4], {{180, 200}, {305, 335}, {390, 420}});
}

double constraint_poz_unit6(const std::vector<double>& P) {
    // NOTA: a tabela original trazia [230, 225] (invertido). Usando [225, 230]
    // corrigido aqui -- confira na sua fonte antes de usar no relatorio.
    return poz_violation(P[5], {{225, 230}, {365, 395}, {430, 455}});
}

double constraint_poz_unit12(const std::vector<double>& P) {
    return poz_violation(P[11], {{30, 40}, {55, 65}});
}

// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------
int main() {
    // 1. Espaco de busca: um GeneBounds por unidade, montado a partir
    //    dos mesmos arrays PMIN/PMAX usados na funcao objetivo.
    std::vector<GeneBounds> bounds;
    bounds.reserve(NUM_UNITS);
    for (int i = 0; i < NUM_UNITS; i++) {
        bounds.push_back(std::pair<double, double>{PMIN[i], PMAX[i]});
    }

    // 2. Restricoes: balanco de carga + zonas proibidas das unidades 2, 5, 6, 12.
    std::vector<GeneticAlgorithm::ConstraintFunction> constraints = {
        constraint_load_balance,
        constraint_poz_unit2,
        constraint_poz_unit5,
        constraint_poz_unit6,
        constraint_poz_unit12
    };

    // 3. Configuracao do algoritmo.
    //    Minimize: queremos o menor custo total possivel.
    //    penalty_weight alto: restricoes de igualdade e zonas proibidas
    //    estreitas exigem penalizacao forte para serem respeitadas.
    GeneticAlgorithm ga(
        GeneticAlgorithm::OptimizationGoal::Minimize,
        GeneticAlgorithm::uniform_crossover,
        constraints,
        8000.0
    );

    // 4. Executa a evolucao. Problema de 15 variaveis com restricoes apertadas
    //    costuma precisar de populacao maior e mais geracoes que os testes
    //    anteriores.
    auto result = ga.evolve(
        /*max_generations=*/ 5000,
        /*population_size=*/ 500,
        /*mutation_rate=*/   0.1,
        /*tournament_size=*/ 10,
        bounds,
        eld_cost
    );

    const Chromosome& best = result.first;
    double best_fitness = result.second;
    auto values = GeneFactory::chromosome_to_doubles(best.get_genes());

    // 5. Reporta o resultado.
    std::cout << "=== Melhor solucao encontrada ===\n";
    double sum = 0.0;
    for (int i = 0; i < NUM_UNITS; i++) {
        std::cout << "P" << (i + 1) << " = " << values[i] << " MW\n";
        sum += values[i];
    }
    std::cout << "\nSoma das potencias: " << sum << " MW (demanda: " << LOAD_DEMAND << " MW)\n";
    std::cout << "Custo bruto (sem penalidade): " << eld_cost(values) << " $/h\n";
    std::cout << "Fitness ajustada (com penalidade): " << best_fitness << "\n\n";

    // 6. Verificacao das zonas proibidas.
    std::cout << "=== Verificacao das zonas proibidas ===\n";
    std::cout << "Unidade 2  (P2=" << values[1] << "): "
              << (poz_violation(values[1], {{185,225},{305,335},{420,450}}) > 0 ? "VIOLADA" : "ok") << "\n";
    std::cout << "Unidade 5  (P5=" << values[4] << "): "
              << (poz_violation(values[4], {{180,200},{305,335},{390,420}}) > 0 ? "VIOLADA" : "ok") << "\n";
    std::cout << "Unidade 6  (P6=" << values[5] << "): "
              << (poz_violation(values[5], {{225,230},{365,395},{430,455}}) > 0 ? "VIOLADA" : "ok") << "\n";
    std::cout << "Unidade 12 (P12=" << values[11] << "): "
              << (poz_violation(values[11], {{30,40},{55,65}}) > 0 ? "VIOLADA" : "ok") << "\n";

    return 0;
}