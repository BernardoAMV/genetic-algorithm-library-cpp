//
// Created by bernardo on 8/21/26.
//

#ifndef GENETIC_ALGORITHM_GENETICTYPES_H
#define GENETIC_ALGORITHM_GENETICTYPES_H

#include <variant>
#include <utility>
#include <cmath>

using Gene = std::variant<double, int, bool>;

using GeneBounds = std::variant
<
    std::pair<double, double>,  // Real: min, max
    std::pair<int, int>,        // Integer: min, max
    std::monostate               // Boolean: sem limites
>;

struct GenSpec {
    GeneBounds kind;
    double min_value = -INFINITY;
    double max_value = INFINITY;
};

#endif //GENETIC_ALGORITHM_GENETICTYPES_H
