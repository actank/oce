#ifndef BUDDYENTRY_H_
#define BUDDYENTRY_H_

#include <vector>
#include <set>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <Ice/Ice.h>
#include <inttypes.h>

namespace xce {
namespace buddy {

const uint32_t MAX_INT = 0xFFFFFFFF;

const uint32_t DESC_UNKNOWN = 0x0;
const uint32_t DESC_FRIEND = 0x10;
const uint32_t DESC_APPLY = 0x01;
const uint32_t DESC_APPLIED = 0x02;
const uint32_t DESC_BLOCK = 0x04;
const uint32_t DESC_BLOCKED = 0x08;

struct Entry {
	uint32_t to;
	uint32_t desc;

	Entry() :
		to(0), desc(0) {
	}

	Entry(uint32_t to, uint32_t d) :
		to(to), desc(d) {
	}

	friend std::ostream& operator<<(std::ostream& os, const Entry& entry) {
		os << "Entry:[" <<entry.to << " " << entry.desc / 0x01000000 << "."
				<< entry.desc % 0x01000000 / 0x00010000 <<"." <<entry.desc
				% 0x00010000 / 0x00000100<<"."<<entry.desc % 0x00000100 << "]";
		return os;
	}
};

//---------------------------------------------------------------------------
struct equal_entry : public std::binary_function<Entry, Entry, bool> {
	bool operator()(const Entry& x, const Entry& y) const {
		return (x.to == y.to);
	}
};

struct less_entry : public std::binary_function<Entry, Entry, bool> {
	bool operator()(const Entry& x, const Entry& y) const {
		return x.to < y.to;
	}
};

struct less_equal_entry : public std::binary_function<Entry, Entry, bool> {
	bool operator()(const Entry& x, const Entry& y) const {
		return x.to <= y.to;
	}
};

struct greater_equal_entry : public std::binary_function<Entry, Entry, bool> {
	bool operator()(const Entry& x, const Entry& y) const {
		return x.to >= y.to;
	}
};

typedef std::vector<Entry> EntryList;
typedef std::set<Entry, less_entry> EntrySet;
typedef std::map<uint32_t, EntryList> EntryListMap;

//---------------------------------------------------------------------------

class EntryListResultBuilder : public Ice::Object {
public:
	void build(const Entry& entry) {
		_result.push_back(entry.to);
	}
	MyUtil::IntSeq result() {
		return _result;
	}
private:
	MyUtil::IntSeq _result;
};
typedef IceUtil::Handle<EntryListResultBuilder> EntryListResultBuilderPtr;

//---------------------------------------------------------------------------

}
}

#endif /*BUDDYENTRY_H_*/
