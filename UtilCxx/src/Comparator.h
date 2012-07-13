#ifndef __COMPARATOR_H__
#define __COMPARATOR_H__

namespace MyUtil
{
using namespace std;

template <class T>
struct Comparator : public binary_function<T, T, bool> {
	bool operator()(T& x, T& y) { 
	    if (funcs.size()) {
    	    for (int i = 0; i < funcs.size(); ++i) {
    	        binary_function<T, T, bool>& func = funcs.at(i);
    	        if (func(x, y)) {
    	            return true;
    	        } else if (!func(x, y)) {
    	            return false;
    	        }
    	    }
    	}
	    return x < y; 
	}
	
	vector<binary_function<T, T, bool> > funcs;
};

};
#endif
