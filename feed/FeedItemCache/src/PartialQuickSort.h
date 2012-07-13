/*
 * partial_quick_sort.h
 *
 *  Created on: Aug 31, 2011
 *      Author: happy.guanghe.ge@renren-inc.com
 */

#ifndef __PARTIAL_QUICK_SORT_H__
#define __PARTIAL_QUICK_SORT_H__


namespace xce {
namespace feed {

template<typename RandomAccessContainer, typename Compare>
struct PartialQuickSort
{
	PartialQuickSort(RandomAccessContainer &v, size_t offset, size_t count, Compare cmp)
		: v_(v), range_left_(offset), range_right_(offset + count - 1), cmp_(cmp) {}

	RandomAccessContainer &v_;
	size_t range_left_;
    size_t range_right_;
	Compare cmp_;

	void sort(size_t left, size_t right) {
	Begin:
		if(left >= right) return;
		size_t l = left; 
	    size_t r = right; 
		size_t mid = (right + left + 1) / 2;

		typename RandomAccessContainer::value_type t = v_[mid];
		v_[mid] = v_[l];

		while(true) {
			while(true) {
				if(l >= r) goto Exit;
				if(!cmp(t, v_[r])) {//找到比mid小的r
					break;
				}
				r--;
			}

			v_[l] = v_[r];//交换赋值
			l++;

			while(true){
				if(l >= r) goto Exit;
				if(!cmp(v_[l], t)) {
					break;
				}
				l++;
			}

			v_[r] = v_[l];
			r--;
		}
	Exit:
		v_[l] = t;
		if(range_left_ < l) {//not range_left_ <= l - 1
			if(range_right_ > l) {
				if(l * 2 > left + right) {//找到元素中间偏右
					sort(l + 1, right);
					right = l - 1;
					goto Begin;
				} else {
					sort(left, l - 1);
					left = l + 1;
					goto Begin;
				}
			} else { //末尾已经排好
				right = l - 1;
				goto Begin;
			}
		}

		if(range_right_ > l) {
			left = l + 1;
			goto Begin;
		}
	}
};//end struct

template<typename RandomAccessContainer>
void partial_quick_sort(RandomAccessContainer &v, size_t offset, size_t count) {
	if(offset >= v.size() || count == 0)
		return;
	PartialQuickSort<RandomAccessContainer, std::less<typename RandomAccessContainer::value_type> > partail_quick_sort(v, offset, count, std::less<typename RandomAccessContainer::value_type>());
	partail_quick_sort.sort(0, v.size() - 1);
	return;
}


template<typename RandomAccessContainer, typename Compare>
void partial_quick_sort(RandomAccessContainer &v, size_t offset, size_t count, Compare cmp) {
	if(offset >= v.size() || count == 0)
		return;
	PartialQuickSort<RandomAccessContainer, Compare> partail_quick_sort(v, offset, count, cmp);
	partail_quick_sort.sort(0, v.size() - 1);
	return;
}

}//end feed
}//end xce

#endif /* PARTIAL_QUICK_SORT_H_ */

