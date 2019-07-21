#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// RAND_MAX is (1 << 31) - 1 on macOS 10.14.5
//#define DEBUG_PRINT

#define MAX_METHOD 8

//#define TOP_K
//#define DIPOLE_K
#ifndef STEP_UNIT
    #define STEP_UNIT 1
#endif
//#define APPEND_TO_FILE

FILE *result;
char *file_names[8] = {"TOP_K_result.txt", "DIPOLE_K_result.txt",
                       "RAND_CLUSTER_K", "uniform_k_result.txt"};
char *file_name;

// tools start
// cmp for qsort
int cmp(const void *a, const void *b){
    return *((int *)a) - *((int *)b);
}

// swaps in A
void swapA(int *A, int i, int j){
    int tmp = A[i];
    A[i] = A[j]; A[j] = tmp;
}

// print A's content
void printA(int *A, int sizeofA){
    for(int i = 0; i < sizeofA; i++)
        printf("%d ", A[i]);
    puts("");
}

// return uniformly distributed number
int uniform(int m){
	return rand() % m;
}

// verifier 
void same_k_select(int *A, int *Q, int *_A, int *_Q, int sizeofA, int sizeofQ){
    for(int i = 0; i < sizeofQ; i++)
    	if(A[Q[i]] != _A[_Q[i]]){
    		puts("invalid");
    		return;
    	}
    puts("valid");
}
// tools end

// random instance start
// set identity permutation
void set_id_permutation(int *A, int sizeofA){
    for(int i = 0; i < sizeofA; i++)
    	A[i] = i;
}

// uniformly arrange k elements in A
void random_k_arrange(int *A, int sizeofA, int k){
	if(k > sizeofA)
		return;
	for(int i = 0; i < k - 1; i++)
		swapA(A, i, i + uniform(sizeofA - i));
}

// random shuffle
void random_shuffle(int *A, int sizeofA){
    random_k_arrange(A, sizeofA, sizeofA); 
}

// random Q
void set_rand_Q(int *Q, int sizeofQ, int sizeofA){
#ifdef TOP_K
    for(int i = 0; i < sizeofQ; i++)
    	Q[i] = i * STEP_UNIT;
#elif DIPOLE_K
    for(int i = 0; i < (sizeofQ >> 1); i++)
    	Q[i] = i * STEP_UNIT;
    for(int i = (sizeofQ >> 1), j = 0; i < sizeofQ; i++, j++)
    	Q[i] = (sizeofA - 1) - (j * STEP_UNIT);
#elif RAND_CLUSTER_K
    Q[0] = uniform(sizeofA - sizeofQ * STEP_UNIT);
    for(int i = 1; i < sizeofQ; i++)
        Q[i] = Q[i - 1] + STEP_UNIT;
#else
	int *tA = (int *)malloc(sizeofA * sizeof(int));
	set_id_permutation(tA, sizeofA);
	random_k_arrange(tA, sizeofA, sizeofQ);
	for(int i = 0; i < sizeofQ; i++)
		Q[i] = tA[i];
	free(tA);
#endif
}
// random instance end

// algorithms start
// GNU library qsort
void GNU_qsort(int *A, int sizeofA, int *Q, int sizeofQ){
    qsort(A, sizeofA, sizeof(int), cmp);
}

// return the element closest to q
int get_closest(int *Q, int a, int b, int q){ // Q[a] <= q <= Q[b]
    if(Q[b] - q > q - Q[a])
        return b;
    return a;
}

// find the element in Q that is the closest to q
int nearest_binary_search(int q, int *Q, int sizeofQ){ 
    if(q == Q[0])
        return 0;
    if(q == Q[sizeofQ - 1])
        return sizeofQ - 1;
    int l = 0, r = sizeofQ, mid;
    while(l < r){
        mid = (l + r) >> 1;
        if(q == Q[mid])
            return mid;
        if(q < Q[mid]){
            if(mid > 0 && q > Q[mid - 1])
                return get_closest(Q, mid - 1, mid, q);
            r = mid;
        }else{
            if(mid < sizeofQ - 1 && q < Q[mid + 1])
                return get_closest(Q, mid, mid + 1, q);
            l = mid + 1;
        }
    }
    return mid;
}

// partition according to the tail
int tail_partition(int *A, int sizeofA){
    int x = A[sizeofA - 1];
    int i = -1;
    for(int j = 0; j < sizeofA - 1; j++)
	    if(A[j] <= x)
            swapA(A, ++i, j);
    swapA(A, i + 1, sizeofA - 1);
    return i + 1;
}

// partition according to a random element
int rand_partition(int *A, int sizeofA){
    int i = uniform(sizeofA);
    swapA(A, i, sizeofA - 1);
    return tail_partition(A, sizeofA);
}

// randomized quick sort util
void rand_quick_sort_util(int *A, int sizeofA){
    if(sizeofA < 2)
        return;
    int q = rand_partition(A, sizeofA);
    rand_quick_sort_util(A, q);
    rand_quick_sort_util(A + q + 1, sizeofA - q - 1);
}

// randomized quick sort
void rand_quick_sort(int *A, int sizeofA, int *Q, int sizeofQ){
	rand_quick_sort_util(A, sizeofA);
}

// randomized selection for an element of certain rank
int rand_select(int *A, int sizeofA, int rank){
    if(sizeofA == 1)
        return A[0];
    int q = rand_partition(A, sizeofA);
    if(rank == q)
        return A[q];
    if(rank < q)
        return rand_select(A, q, rank);
    return rand_select(A + q + 1, sizeofA - q - 1, rank - q - 1);
}

// divide-and-conquer strategy for solving the rank vector selection problem
// mode: 1.median-of-Q. 2.nearest-median-of-A.
void quick_k_select_util(int mode, int *A, int sizeofA, int *Q, int sizeofQ){
    if(sizeofA < 1)
        return;
    if(sizeofQ < 1)
        return;
    int split_Q;
	switch(mode){
        case 0: split_Q = sizeofQ >> 1; break;
        case 1: split_Q = nearest_binary_search(sizeofA >> 1, Q, sizeofQ); break;
    }
    rand_select(A, sizeofA, Q[split_Q]);
    int x = Q[split_Q];
    for(int i = split_Q + 1; i < sizeofQ; i++)
        Q[i] -= (x + 1);
    quick_k_select_util(mode, A, Q[split_Q], Q, split_Q);
    quick_k_select_util(mode, A + Q[split_Q] +1, sizeofA - Q[split_Q] - 1,
        Q + split_Q + 1, sizeofQ - split_Q - 1);
}

// median-of-Q
void median_of_Q(int *A, int sizeofA, int *Q, int sizeofQ){
    quick_k_select_util(0, A, sizeofA, Q, sizeofQ);
}

// nearest-median-of-A
void nearest_median_of_A(int *A, int sizeofA, int *Q, int sizeofQ){
	quick_k_select_util(1, A, sizeofA, Q, sizeofQ);
}

// purely randomized implicit selection
void rand_k_select(int *A, int sizeofA, int *Q, int sizeofQ){
    if(sizeofA < 1)
        return;
    if(sizeofQ < 1)
        return;
    if(sizeofQ == 1){
        rand_select(A, sizeofA, Q[0]);
        return;
    }

    int pivot = rand_partition(A, sizeofA);
    int split_Q = nearest_binary_search(pivot, Q, sizeofQ);

    if(pivot < Q[split_Q]){
        for(int i = split_Q; i < sizeofQ; i++)
            Q[i] -= (pivot + 1);
        rand_k_select(A, pivot, Q, split_Q);
        rand_k_select(A + pivot + 1, sizeofA - pivot - 1, 
            Q + split_Q, sizeofQ - split_Q);
    }else if(pivot > Q[split_Q]) {
        for(int i = split_Q + 1; i < sizeofQ; i++)
            Q[i] -= (pivot + 1);
        rand_k_select(A, pivot, Q, split_Q + 1);
        rand_k_select(A + pivot + 1, sizeofA - pivot - 1,
            Q + split_Q + 1, sizeofQ - split_Q - 1);
    }else{
        for(int i = split_Q + 1; i < sizeofQ; i++)
            Q[i] -= (pivot + 1);
        rand_k_select(A, pivot, Q, split_Q);
        rand_k_select(A + pivot + 1, sizeofA - pivot - 1,
            Q + split_Q + 1, sizeofQ - split_Q - 1);
    }
}

// algorithms end

// control block start
void control_block(int *A, int sizeofA, int *Q, int sizeofQ, 
	void (*method) (int*, int, int*, int), char *method_name){
    clock_t clock_start, clock_end;

    clock_start = clock();
    method(A, sizeofA, Q, sizeofQ);
    clock_end = clock();

    clock_t clock_diff = clock_end - clock_start;

    printf("%lu clock ticks for %s.\n", clock_diff, method_name);

#ifdef APPEND_TO_FILE
    fprintf(result, "%lu clock ticks for %s.\n", clock_diff, method_name);
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
#elif RAND_CLUSTER_K
    file_name = file_names[2];
#else
    file_name = file_names[3];
#endif

    result = fopen(file_name, "a");
    fprintf(result, "Step unit is %d\n", STEP_UNIT);
#endif
    int method_n = 3, *methods_A[MAX_METHOD], *methods_Q[MAX_METHOD];
	char *methods_name[MAX_METHOD] = {"Randomized k selection",
	    "Nearest-median-of-A", "Median-of-Q", "Quicksort"};
	void (*methods[MAX_METHOD]) (int*, int, int*, int) = {
		&rand_k_select, &nearest_median_of_A, &median_of_Q, &rand_quick_sort};

	while(~scanf("%d%d", &sizeofA, &sizeofQ)){
        int *A = (int *)malloc(sizeofA * sizeof(int));
		for(int i = 0; i < method_n; i++)
			methods_A[i] = (int *)malloc(sizeofA * sizeof(int));

		set_id_permutation(A, sizeofA);
		random_shuffle(A, sizeofA);

#ifdef DEBUG_PRINT
        printA(A, sizeofA);
#endif

		for(int i = 0; i < method_n; i++)
			for(int j = 0; j < sizeofA; j++)
				methods_A[i][j] = A[j];

        int *Q = (int *)malloc(sizeofQ * sizeof(int));
        for(int i = 0; i < method_n; i++)
        	methods_Q[i] = (int *)malloc(sizeofQ * sizeof(int));

		set_rand_Q(Q, sizeofQ, sizeofA);
		qsort(Q, sizeofQ, sizeof(int), cmp);

#ifdef DEBUG_PRINT
        printA(Q, sizeofQ);
#endif

        for(int i = 0; i < method_n; i++)
        	for(int j = 0; j < sizeofQ; j++)
        		methods_Q[i][j] = Q[j];

#ifdef APPEND_TO_FILE
        fprintf(result, "Input size: %d %d\n", sizeofA, sizeofQ);
#endif
        control_block(A, sizeofA, Q, sizeofQ, &GNU_qsort, "GNU quicksort");

        for(int i = 0; i < method_n; i++)
        	control_block(methods_A[i], sizeofA, methods_Q[i], sizeofQ,
        		methods[i], methods_name[i]);

// verifying start
        for(int i = 0; i < method_n; i++)
        	same_k_select(A, Q, methods_A[i], Q, sizeofA, sizeofQ);
// verifying end

	    free(A); free(Q);
	    for(int i = 0; i < method_n; i++)
	    	free(methods_A[i]), free(methods_Q[i]);
	}
	fclose(result);
}