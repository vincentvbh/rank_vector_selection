// Algorithm selection extension implementation -*- C++ -*-

#include <algorithm>

// median
inline const int &__median(const int &__a, const int &__b, const int &__c){
    if(__a < __b)
        if(__b < __c)
            return __b;
        else if(__a < __c)
            return __c;
        else
            return __a;
    else if(__a < __c)
        return __a;
    else if(__b < __c)
        return __c;
    else
        return __b;

}

// lg n
template<typename _Size>
    inline _Size
    __lg(_Size __n){
    	_Size __k;
    	for(__k = 0; __n != 1; __n >>= 1)
            __k++;
        return __k;
    }

// compare the ratio of lg's
template<typename _Size>
    inline bool
    __lg_racing(_Size __n1, int __speed1, _Size __n2, int __speed2){
        if(__speed1 == __speed2)
            return __n1 > __n2 / 2;
        while(__n1 > 0 && __n2 > 0){
            __n1 >>= __speed1;
            __n2 >>= __speed2;
        }
        return __n1 != 0;
    }

// partition
template<typename _RandomAccessIterator, typename _Tp>
    _RandomAccessIterator 
    __unguarded_partition(_RandomAccessIterator __first, 
    	_RandomAccessIterator __last, _Tp __pivot){
    	while(true){
    		while(*__first < __pivot)
    			++__first;
    		--__last;
    		while(__pivot < *__last)
    			--__last;
    		if(!(__first < __last))
    			return __first;
    		std::iter_swap(__first, __last);
    		++__first;
    	}
    }


// multi-select loop
template<typename _RandomAccessIterator>
    void 
    __intro_multi_select_loop(_RandomAccessIterator __A_first, 
        _RandomAccessIterator __A_last, _RandomAccessIterator __Q_first,
        _RandomAccessIterator __Q_last, int __depth_limit){

        while((__A_last - __A_first) > 1 && (__Q_last - __Q_first) > 0){
            if(__Q_first + 1 == __Q_last){
                std::nth_element(__A_first, __A_first + *__Q_first, __A_last);
                return;
            }
            if(__lg_racing(__Q_last - __Q_first, 1, __A_last - __A_first, 1)){
                std::sort(__A_first, __A_last);
                return;
            }
            if(__depth_limit == 0){
                std::partial_sort(__A_first, __A_last, __A_last);
                return;
            }
            --__depth_limit;
            _RandomAccessIterator __A_pivot = 
                __unguarded_partition(__A_first, __A_last,
                    __median(*__A_first, 
                             *(__A_first + (__A_last - __A_first) / 2),
                             *(__A_last - 1)));
            if((__A_pivot - __A_first) > *(__Q_last - 1)){
                __A_last = __A_pivot;
            }else if((__A_pivot - __A_first) < *__Q_first){
            	for(_RandomAccessIterator i = __Q_first; i != __Q_last; ++i)
            		(*i) -= (__A_pivot - __A_first);
                __A_first = __A_pivot;
            }else{
                _RandomAccessIterator __Q_split =
                    std::lower_bound(__Q_first, __Q_last, (__A_pivot - __A_first));
                for(_RandomAccessIterator i = __Q_split; i != __Q_last; ++i)
                    (*i) -= (__A_pivot - __A_first);
                __intro_multi_select_loop(__A_pivot, __A_last,
                    __Q_split, __Q_last, __depth_limit);
                __A_last = __A_pivot;
                __Q_last = __Q_split;
            }
        }
    }

// multi-select
template<typename _RandomAccessIterator>
    inline void
    __multi_select(_RandomAccessIterator __A_first, _RandomAccessIterator __A_last,
    	_RandomAccessIterator __Q_first, _RandomAccessIterator __Q_last){
    	__intro_multi_select_loop(__A_first, __A_last, 
    		__Q_first, __Q_last, __lg(__A_last - __A_first) * 2);
    }

