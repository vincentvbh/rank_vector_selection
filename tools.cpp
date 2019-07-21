#include "tools.h"

// return uniformly distributed number
int uniform(int m){
    return rand() % m;
}

// print ranged A
void print_range(std::vector<int>::iterator __first, std::vector<int>::iterator __last){
    for(std::vector<int>::iterator i = __first; i != __last; ++i)
        std::cout << *i << ' ';
    std::cout << std::endl;
}

// print A's content
void printA(std::vector<int> &A){
    print_range(A.begin(), A.end());
}

// verifier 
void same_k_select(std::vector<int> &A, std::vector<int> &_A, std::vector<int> &Q){
    std::vector<int>::iterator i;
    for(i = Q.begin(); i != Q.end(); ++i)
    	if(A[*i] != _A[*i]){
    		puts("invalid");
    		return;
    	}
    puts("valid");
}

// simplified verifier
void restricted_verifier(std::vector<int> &A, std::vector<int> &Q){
    for(std::vector<int>::iterator i = Q.begin(); i != Q.end(); ++i){
        if(A[*i] != *i){
            puts("invalid");
            return;
        }
    }
    puts("valid");
}
// tools end

// set identity permutation
void set_id_permutation(std::vector<int> &A, int sizeofA){
    for(int i = 0; i < sizeofA; i++)
        A.push_back(i);
}

// uniformly arrange k elements in A
void random_k_arrange(std::vector<int> &A, int k){
	if(k > A.size()){
        std::random_shuffle(A.begin(), A.end());
        return;
    }
	for(int i = 0; i < k - 1; i++)
		std::swap(A[i], A[i + uniform(A.size() - i)]);
}

// uniformly distributed Q
void set_uniform_Q(std::vector<int> &Q, int sizeofQ, int sizeofA){
    std::vector<int> A;
    set_id_permutation(A, sizeofA);
    random_k_arrange(A, sizeofQ);
    Q.assign(A.begin(), A.begin() + sizeofQ);
}

// random Q
void set_rand_Q(std::vector<int> &Q, int sizeofQ, int sizeofA){
#ifdef TOP_K
    for(int i = 0; i < sizeofQ; i++)
        Q.push_back(i * STEP_UNIT);
#elif DIPOLE_K
    for(int i = 0; i < (sizeofQ >> 1); i++)
        Q.push_back(i * STEP_UNIT);
    for(int i = (sizeofQ >> 1), j = 0; i < sizeofQ; i++, j++)
        Q.push_back((sizeofA - 1) - (j * STEP_UNIT));
#else
    set_uniform_Q(Q, sizeofQ, sizeofA);
#endif
}

// find the element in Q that is the closest to q
std::vector<int>::iterator nearest_binary_search(std::vector<int>::iterator Q_start,
       std::vector<int>::iterator Q_end, int q){
    if(Q_start + 1 == Q_end)
        return Q_start;
    if(*(Q_end - 1) <= q)
        return Q_end - 1;
    if(q <= *Q_start)
        return Q_start;
    std::vector<int>::iterator low_indx;
    low_indx = std::lower_bound(Q_start, Q_end, q);
    if(q == *low_indx)
        return low_indx;
    if(*low_indx + *(low_indx - 1) > (q << 1))
        return low_indx - 1;
    return low_indx;
}