cmake_minimum_required(VERSION 3.5)

project(Polynomial)

set(CMAKE_CXX_STANDARD 17)
############################################################
# Create a library
############################################################

#Generate the static library from the library sources
add_library(Polynomial STATIC
        src/polynomial.cpp
        )

target_include_directories(Polynomial
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        )


############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(PolynomialCalculator
        src/main.cpp
        )

# link the new Polynomial target with the PolynomialCalculator target
target_link_libraries(PolynomialCalculator
        PRIVATE
        Polynomial
        )