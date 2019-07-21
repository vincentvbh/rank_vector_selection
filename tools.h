#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <utility>

#ifndef STEP_UNIT
#define STEP_UNIT 1
#endif

// return uniformly distributed number
int uniform(int m);

// print ranged A
void print_range(std::vector<int>::iterator, std::vector<int>::iterator);

// print A's content
void printA(std::vector<int>&);

// verifier 
void same_k_select(std::vector<int>&, std::vector<int>&, std::vector<int>&);

// simplified verifier
void restricted_verifier(std::vector<int>&, std::vector<int>&);

// set identity permutation
void set_id_permutation(std::vector<int>&, int);

// uniformly arrange k elements in A
void random_k_arrange(std::vector<int>&, int);

// uniformly distributed Q
void set_uniform_Q(std::vector<int>&, int, int);

// random Q
void set_rand_Q(std::vector<int>&, int, int);

// find the element in Q that is the closest to q
std::vector<int>::iterator nearest_binary_search(std::vector<int>::iterator,
        std::vector<int>::iterator, int q);