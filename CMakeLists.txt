cmake_minimum_required(VERSION 4.3)
project(genetic-algorithm-library-cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(genetic-algorithm-library-cpp
        main.cpp
        Chromosome.cpp
        headers/Chromosome.h
        headers/GeneticTypes.h
        Population.cpp
        headers/Population.h
        GeneFactory.cpp
        headers/GeneFactory.h
        GeneticAlgorithm.cpp
        headers/GeneticAlgorithm.h
)
