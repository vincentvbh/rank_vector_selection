#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tools.h"
#include "tools_extension.h"

// RAND_MAX is (1 << 31) - 1 on macOS 10.14.5
//#define DEBUG_PRINT

#define MAX_METHOD 8

//#define TOP_K
//#define DIPOLE_K
//#define PRINT_TO_TERMINAL
//#define APPEND_TO_FILE
//#define LARGE_TEST
#ifndef TEST_N
#define TEST_N 500
#endif

#ifdef LARGE_TEST
clock_t sort_total_cost, sort_min_cost, sort_max_cost;
clock_t total_cost[MAX_METHOD], min_cost[MAX_METHOD], max_cost[MAX_METHOD];
#endif

FILE *result, *large_result;
char *file_names[8] = {"TOP_K_result.txt", "DIPOLE_K_result.txt",
                       "uniform_k_result.txt"};
char *file_name;

// median-of-Q template
template<typename _RandomAccessIterator>
    void 
    __median_of_Q_loop(_RandomAccessIterator __A_first, 
        _RandomAccessIterator __A_last, _RandomAccessIterator __Q_first,
        _RandomAccessIterator __Q_last){

        while((__A_last - __A_first) > 1 && (__Q_last - __Q_first) > 0){
            if(__Q_first + 1 == __Q_last){
                std::nth_element(__A_first, __A_first + *__Q_first, __A_last);
                return;
            }
            if(__lg_racing(__Q_last - __Q_first, 1, __A_last - __A_first, 1)){
                std::sort(__A_first, __A_last);
                return;
            }
            _RandomAccessIterator __Q_split =
                __Q_first + (__Q_last - __Q_first) / 2;
            std::nth_element(__A_first, __A_first + *__Q_split, __A_last);
            for(_RandomAccessIterator i = __Q_split + 1; i != __Q_last; ++i)
                (*i) -= (*__Q_split + 1);
            __median_of_Q_loop(__A_first + *__Q_split + 1, __A_last,
                __Q_split + 1, __Q_last);
            __A_last = __A_first + *__Q_split;
            __Q_last = __Q_split;
        }
    }

// median-of-Q util
void median_of_Q_util(std::vector<int>::iterator A_start,
       std::vector<int>::iterator A_end, std::vector<int>::iterator Q_start,
       std::vector<int>::iterator Q_end){
    if(A_start == A_end)
        return;
    if(Q_start == Q_end)
        return;
    std::vector<int>::iterator split_Q = Q_start + (Q_end - Q_start) / 2;
    int x = *split_Q;
    std::nth_element(A_start, A_start + x, A_end);
    for(std::vector<int>::iterator i = split_Q + 1; i != Q_end; ++i)
        (*i) -= (x + 1);
    median_of_Q_util(A_start, A_start + x, Q_start, split_Q);
    median_of_Q_util(A_start + x + 1, A_end, split_Q + 1, Q_end);
}

// nearest-median-of-A template
template<typename _RandomAccessIterator>
    void 
    __nearest_median_of_A_loop(_RandomAccessIterator __A_first, 
        _RandomAccessIterator __A_last, _RandomAccessIterator __Q_first,
        _RandomAccessIterator __Q_last){

        while((__A_last - __A_first) > 1 && (__Q_last - __Q_first) > 0){
            if(__Q_first + 1 == __Q_last){
                std::nth_element(__A_first, __A_first + *__Q_first, __A_last);
                return;
            }
            if(__lg_racing(__Q_last - __Q_first, 1, __A_last - __A_first, 1)){
                std::sort(__A_first, __A_last);
                return;
            }
            _RandomAccessIterator __Q_split =
                std::lower_bound(__Q_first, __Q_last, (__A_last - __A_first) / 2);
            if(__Q_split == __Q_last)
                --__Q_split;
            else if(__Q_split != __Q_first &&
               __A_first + *(__Q_split - 1) > __A_last - *__Q_split)
                --__Q_split;
            std::nth_element(__A_first, __A_first + *__Q_split, __A_last);
            for(_RandomAccessIterator i = __Q_split + 1; i != __Q_last; ++i)
                (*i) -= (*__Q_split + 1);
            __nearest_median_of_A_loop(__A_first + *__Q_split + 1, __A_last,
                __Q_split + 1, __Q_last);
            __A_last = __A_first + *__Q_split;
            __Q_last = __Q_split;
        }
    }

// nearest-median-of-A util
void nearest_median_of_A_util(std::vector<int>::iterator A_start,
       std::vector<int>::iterator A_end, std::vector<int>::iterator Q_start,
       std::vector<int>::iterator Q_end){
    if(A_start == A_end)
        return;
    if(Q_start == Q_end)
        return;
    if(__lg_racing(Q_end - Q_start, 1, A_end - A_start, 1)){
        std::sort(A_start, A_end);
        return;
    }
    std::vector<int>::iterator split_Q;
    split_Q = nearest_binary_search(Q_start, Q_end, (A_end - A_start) >> 1);
    int x = *split_Q;
    std::nth_element(A_start, A_start + x, A_end);
    for(std::vector<int>::iterator i = split_Q + 1; i != Q_end; ++i)
        (*i) -= (x + 1);
    nearest_median_of_A_util(A_start, A_start + x, Q_start, split_Q);
    nearest_median_of_A_util(A_start + x + 1, A_end, split_Q + 1, Q_end);
}

// median-of-Q
void median_of_Q(std::vector<int> &A, std::vector<int> &Q){
//    median_of_Q_util(A.begin(), A.end(), Q.begin(), Q.end());
    __median_of_Q_loop(A.begin(), A.end(), Q.begin(), Q.end());
}

// nearest-median-of-A
void nearest_median_of_A(std::vector<int> &A, std::vector<int> &Q){
//    nearest_median_of_A_util(A.begin(), A.end(), Q.begin(), Q.end());
    __nearest_median_of_A_loop(A.begin(), A.end(), Q.begin(), Q.end());
}

// purely partition
void purely_partition(std::vector<int> &A, std::vector<int> &Q){
    __multi_select(A.begin(), A.end(), Q.begin(), Q.end());
}
// algorithms end

// STL sort
void STL_sort(std::vector<int> &A, std::vector<int> &Q){
	std::sort(A.begin(), A.end());
}

// control block start
void control_block(int indx, std::vector<int> &A, std::vector<int> &Q, 
	void (*method) (std::vector<int>&, std::vector<int>&), char *method_name){

    clock_t clock_start, clock_end;

    clock_start = clock();
    method(A, Q);
    clock_end = clock();

    clock_t clock_diff = clock_end - clock_start;

#ifdef PRINT_TO_TERMINAL
    printf("%lu clocks for %s.\n", clock_diff, method_name);
#endif

#ifdef APPEND_TO_FILE
    fprintf(result, "%lu clocks for %s.\n", clock_diff, method_name);
#endif

#ifdef LARGE_TEST
    total_cost[indx] += clock_diff;
    if(clock_diff < min_cost[indx])
    	min_cost[indx] = clock_diff;
    if(clock_diff > max_cost[indx])
    	max_cost[indx] = clock_diff;
#endif
}

void stl_sort(std::vector<int> &A, std::vector<int> &Q){
    clock_t clock_start, clock_end;

    clock_start = clock();
    std::sort(A.begin(), A.end());
    clock_end = clock();

    clock_t clock_diff = clock_end - clock_start;

#ifdef PRINT_TO_TERMINAL
    printf("%lu clocks for STL sort.\n", clock_diff);
#endif

#ifdef APPEND_TO_FILE
    fprintf(result, "%lu clocks for STL sort.\n", clock_diff);
#endif

#ifdef LARGE_TEST
    sort_total_cost += clock_diff;
    if(clock_diff < sort_min_cost)
    	sort_min_cost = clock_diff;
    if(clock_diff > sort_max_cost)
    	sort_max_cost = clock_diff;
#endif
}
// control block end

int main(){
	int sizeofA, sizeofQ;

#ifdef APPEND_TO_FILE
#ifdef TOP_K
    file_name = file_names[0];
#elif DIPOLE_K
    file_name = file_names[1];
#else
    file_name = file_names[2];
#endif

    result = fopen(file_name, "a");
    fprintf(result, "Step unit is %d\n", STEP_UNIT);
#endif

#ifdef LARGE_TEST
    large_result = fopen("large_data_test.txt", "a");
#endif

    int method_n = 3;
    std::vector<int> methods_A[MAX_METHOD], methods_Q[MAX_METHOD];
	char *methods_name[MAX_METHOD] = {"Purely-partition with partially iterative template",
        "Nearest-median-of-A with partially iterative template", 
        "Median-of-Q with partially iterative template"};
	void (*methods[MAX_METHOD]) (std::vector<int>&, std::vector<int>&) = {
		&purely_partition, &nearest_median_of_A, &median_of_Q};

	while(~scanf("%d%d", &sizeofA, &sizeofQ)){
#ifdef LARGE_TEST

#ifdef TOP_K
        fprintf(large_result, "Top-k\n");
#elif DIPOLE_K
        fprintf(large_result, "Dipole-k\n");
#else
        fprintf(large_result, "Uniform k\n");
#endif
        fprintf(large_result, "Input size: %d %d\n", sizeofA, sizeofQ);

		sort_total_cost = 0;
		sort_min_cost = 1 << 20;
		sort_max_cost = 0;
		for(int i = 0; i < method_n; i++){
			total_cost[i] = 0;
			min_cost[i] = 1 << 20;
			max_cost[i] = 0;
		}
#endif

#ifdef LARGE_TEST
        int test_count = 0;
        while(test_count++ < TEST_N){
#endif
        std::vector<int> A, Q;

		set_id_permutation(A, sizeofA);
		std::random_shuffle(A.begin(), A.end());
#ifdef PRINT_TO_TERMINAL
		printf("A contains %d elements.\n", int(A.size()));
#endif

#ifdef DEBUG_PRINT
        printA(A);
#endif
		for(int i = 0; i < method_n; i++)
            methods_A[i].assign(A.begin(), A.end());

		set_rand_Q(Q, sizeofQ, sizeofA);
		std::sort(Q.begin(), Q.end());
#ifdef PRINT_TO_TERMINAL
        printf("Q contains %d elemtents.\n", int(Q.size()));
#endif

#ifdef DEBUG_PRINT
        printA(Q);
#endif

        for(int i = 0; i < method_n; i++)
            methods_Q[i].assign(Q.begin(), Q.end());

#ifdef APPEND_TO_FILE
        fprintf(result, "Input size: %d %d\n", sizeofA, sizeofQ);
#endif
        stl_sort(A, Q);

        for(int i = 0; i < method_n; i++)
        	control_block(i, methods_A[i], methods_Q[i], methods[i], methods_name[i]);

#ifdef PRINT_TO_TERMINAL
        for(int i = 0; i < method_n; i++)
        	restricted_verifier(methods_A[i], Q);
#endif

	    A.clear(); Q.clear();
	    for(int i = 0; i < method_n; i++){
            methods_A[i].clear();
            methods_Q[i].clear();
        }
#ifdef LARGE_TEST
        printf("Test %d complete.\n", test_count);
        }
        printf("%lu averaged clocks for STL sort\n", sort_total_cost / TEST_N);
        for(int i = 0; i < method_n; i++)
            printf("%lu averaged clocks for %s\n", total_cost[i] / TEST_N, methods_name[i]);
        printf("%lu minimum clocks for STL sort\n", sort_min_cost);
        for(int i = 0; i < method_n; i++)
            printf("%lu minimum clocks for %s\n", min_cost[i], methods_name[i]);
        printf("%lu maximum clocks for STL sort\n", sort_max_cost);
        for(int i = 0; i < method_n; i++)
        	printf("%lu maximum clocks for %s\n", max_cost[i], methods_name[i]);
        
        fprintf(large_result, "%lu averaged clocks for STL sort\n", sort_total_cost / TEST_N);
        for(int i = 0; i < method_n; i++)
            fprintf(large_result, "%lu averaged clocks for %s\n", total_cost[i] / TEST_N, methods_name[i]);
        fprintf(large_result, "%lu minimum clocks for STL sort\n", sort_min_cost);
        for(int i = 0; i < method_n; i++)
            fprintf(large_result, "%lu minimum clocks for %s\n", min_cost[i], methods_name[i]);
        fprintf(large_result, "%lu maximum clocks for STL sort\n", sort_max_cost);
        for(int i = 0; i < method_n; i++)
            fprintf(large_result, "%lu maximum clocks for %s\n", max_cost[i], methods_name[i]);
        fclose(large_result);
#endif
	}
	fclose(result);
}