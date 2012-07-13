#include "BuddyEntry.h"
#include <boost/utility.hpp>
#include <boost/intrusive_ptr.hpp>
#include "ServiceI.h" // This is only for using LOG.
#include "QueryRunner.h"

using namespace boost;
using namespace std;
using namespace com::xiaonei::service::buddy;
using namespace com::xiaonei::xce;

// **************************************************************************
/*
BuddyDesc translateDesc(uint32_t desc) {
	//BuddyCoreManager contains a copy of these code
	//Remember changing both.
	MCE_DEBUG("Converting desc " << desc);
	switch (desc & DESC_MASK) {
	case DESC_UNKNOWN:
		MCE_WARN("DESC_UNKNOWN");
		return NoPath;
	case DESC_FRIEND:
		return Friend;
	case DESC_APPLY:
		return Apply;
	case DESC_APPLIED:
		return Applied;
	case DESC_BLOCK:
		return Block;
	case DESC_BLOCKED:
		return Blocked;
	case DESC_BLOCK | DESC_BLOCKED:
		return BiBlock;
	default:
		MCE_WARN("default switch. This should not be seen.");
		return NoPath;
	}
}
*/
// **************************************************************************

EntryListHolder::EntryListHolder(uint32_t from, EntryList list) :
	_from(from), _list(list) {
}

EntryListHolder::~EntryListHolder() {
}

void EntryListHolder::add(const Entry& entry) {
	//	dump();
	MCE_TRACE("EntrySegment::add " << entry);
	IceUtil::Mutex::Lock lock(*this);
	EntryList::iterator it = lower_bound(_list.begin(), _list.end(), entry, less_entry());
	if (it != _list.end() && it->to == entry.to) {
		MCE_TRACE("Segment::add1 " << *it);
		it->desc = entry.desc;
	} else {
		MCE_TRACE("Segment::add2 " << entry);
		_list.insert(it, entry);
	}
}

void EntryListHolder::remove(const Entry& key, const Entry& mask) {
	MCE_TRACE("Segment::del " << key);
	//	dump();

	Entry infoBegin;
	infoBegin.to = key.to & mask.to;
	infoBegin.desc = key.desc & mask.desc;

	IceUtil::Mutex::Lock lock(*this);
	EntryList::iterator itBegin = lower_bound(_list.begin(), _list.end(), infoBegin, less_entry());
	
	while (itBegin != _list.end() && equals(key, *itBegin, mask) ) {
                itBegin = _list.erase(itBegin);
        }
}

void EntryListHolder::set(const Entry& key, const Entry& keyMask, const Entry& value, const Entry& valueMask) {
	MCE_TRACE("Segment::set " << key << " / " << keyMask << " " << value << " / " << valueMask);
	if ( (keyMask.to | valueMask.to) != MAX_INT) {
		MCE_WARN("Segment::set not ready.");
		return;
	}
	//	dump();

	Entry infoBegin;
	infoBegin.to = key.to & keyMask.to;
	infoBegin.desc = key.desc & keyMask.desc;

	Entry infoEnd;
	infoEnd.to =~keyMask.to | key.to;
	infoEnd.desc = ~keyMask.desc | key.desc;

	IceUtil::Mutex::Lock lock(*this);
	EntryList::iterator itBegin = lower_bound(_list.begin(), _list.end(), infoBegin, less_entry());
	EntryList::iterator itEnd = lower_bound(itBegin, _list.end(), infoEnd, less_equal_entry());
	for (; itBegin != itEnd; ++itBegin) {
		MCE_TRACE("set segment " << *itBegin )
		if (equals(key, *itBegin, keyMask)) {
			itBegin->desc = (itBegin->desc & ~valueMask.desc) | (value.desc
					& valueMask.desc);
			MCE_TRACE("seted entry " << *itBegin );
		}
		else {
			equalsTrace(key, *itBegin, keyMask);
		}
	}
}

EntryList EntryListHolder::get(const Entry& key) {
	MCE_TRACE("Segment::get " << key);
	//	dump();
	IceUtil::Mutex::Lock lock(*this);
	EntryList::iterator it = find_if(_list.begin(), _list.end(), bind2nd(equal_entry(), key));

	EntryList result;
	if (it!=_list.end()) {
		result.push_back(*it);
	}
	return result;
}

EntryList EntryListHolder::get(const Entry& key, const Entry& mask) {
	MCE_TRACE("Segment::get " << key << " / " << mask);
	//	dump();

	Entry infoBegin((key.to & mask.to), (key.desc & mask.desc));

	Entry infoEnd((key.to|~mask.to), (key.desc|~mask.desc));

	EntryList result;
	IceUtil::Mutex::Lock lock(*this);
	EntryList::iterator itBegin = lower_bound(_list.begin(), _list.end(), infoBegin, less_entry());
	if (itBegin != _list.end()) {
		MCE_TRACE("infoBegin: " << infoBegin << " itBegin: " << *itBegin);
		EntryList::iterator itEnd = lower_bound(itBegin, _list.end(), infoEnd, less_equal_entry());
		if (itEnd != _list.end()) {
			MCE_TRACE("infoEnd: " << infoEnd << " itEnd: " << *itEnd);
		} else {
			MCE_TRACE("infoEnd: " << infoEnd << " itEnd: EOF");
		}
		while (itBegin != _list.end() && itBegin != itEnd) {
			if (equals(key, *itBegin, mask) && itBegin->desc != MAX_INT) {
				MCE_TRACE("found " << *itBegin);
				result.push_back(*itBegin);
			} else {
				equalsTrace(key, *itBegin, mask);
			}
			++itBegin;
		}
	} else {
		MCE_TRACE("not found");
	}
	return result;
}

void EntryListHolder::get(const Entry& key, const Entry& mask, const EntryListResultBuilderPtr& builder) {
	MCE_TRACE("Segment::get2 " << key << " / " << mask);
	//	dump();

	Entry infoBegin((key.to & mask.to), (key.desc & mask.desc));

	Entry infoEnd((key.to|~mask.to), (key.desc|~mask.desc));

	IceUtil::Mutex::Lock lock(*this);
	EntryList::iterator itBegin = lower_bound(_list.begin(), _list.end(), infoBegin, less_entry());
	if (itBegin != _list.end()) {
		MCE_TRACE("infoBegin: " << infoBegin << " itBegin: " << *itBegin);
		EntryList::iterator itEnd = lower_bound(itBegin, _list.end(), infoEnd, less_equal_entry());
		if (itEnd != _list.end()) {
			MCE_TRACE("infoEnd: " << infoEnd << " itEnd: " << *itEnd);
		} else {
			MCE_TRACE("infoEnd: " << infoEnd << " itEnd: EOF");
		}
		while (itBegin != _list.end() && itBegin != itEnd) {
			if (equals(key, *itBegin, mask) && itBegin->desc != MAX_INT) {
				MCE_TRACE("found " << *itBegin);
				builder->build(*itBegin);
			} else {
				equalsTrace(key, *itBegin, mask);
			}
			++itBegin;
		}
	} else {
		MCE_TRACE("not found");
	}
}

EntryList EntryListHolder::getAll() {
	IceUtil::Mutex::Lock lock(*this);
	return _list;
}

//
//void EntryListHolder::dump() {
//	IceUtil::Mutex::Lock lock(*this);
//	MCE_TRACE("Dump _entries: ");
//	for (EntryList::iterator it = _list.begin(); it!=_list.end(); ++it) {
//		MCE_TRACE(*it);
//	}
//	MCE_TRACE("Dump end. ");
//}

bool EntryListHolder::equals(const Entry& l, const Entry& r, const Entry& mask) {
	return ( ((l.to & mask.to) == (r.to & mask.to)) && ((l.desc & mask.desc) == (r.desc & mask.desc)) );
}

void EntryListHolder::equalsTrace(const Entry& l, const Entry& r, const Entry& mask) {
	MCE_TRACE("WhyMaskMatch: " << "\n\ttoId[" << (l.to & mask.to)<<"=" <<(r.to & mask.to) << "\n\tdesc[" << (l.desc
			& mask.desc)<<"=" <<(r.desc & mask.desc));
}
