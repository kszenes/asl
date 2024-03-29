#pragma once

#include <string>
#include "complex.h"
#include <iostream>

typedef void(*comp_func)(complex_t* x, double* y, int n);
void add_function(comp_func f, std::string name, int flop);