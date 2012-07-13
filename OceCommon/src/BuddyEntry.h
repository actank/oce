#ifndef BUDDYENTRY_H_
#define BUDDYENTRY_H_

#include <vector>
#include <set>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include "Singleton.h"
#include <Ice/Ice.h>
#include "BuddyCommon.h"
#include <inttypes.h>

#include "ObjectCacheI.h"
#include "QueryRunner.h"

namespace xce {

namespace buddy {

const static uint32_t MAX_INT = 0xFFFFFFFF;

const static uint32_t DESC_MASK = 0x1F;
const static uint32_t DESC_MASK_FRIEND = 0x10;
const static uint32_t DESC_MASK_APPLY = 0x03;
const static uint32_t DESC_MASK_BLOCK = 0x04;
const static uint32_t DESC_MASK_BLOCKED = 0x08;

const static uint32_t DESC_UNKNOWN = 0x0;
const static uint32_t DESC_FRIEND = 0x10;
const static uint32_t DESC_APPLY = 0x01;
const static uint32_t DESC_APPLIED = 0x02;
const static uint32_t DESC_BLOCK = 0x04;
const static uint32_t DESC_BLOCKED = 0x08;

class BuddyDescHelper{
public:
	static BuddyDesc oppositeDesc(BuddyDesc desc) {
		switch (desc) {
                case NoPath:
                        return NoPath;
                case Friend:
                        return Friend;
                case Apply:
                        return Applied;
                case Applied:
                        return Apply;
                case Block:
                        return Blocked;
                case Blocked:
                        return Block;
                case BiBlock:
                        return BiBlock;
                default:
                        MCE_WARN("default switch. This should not be seen.");
                        return NoPath;
                }
	}

	static BuddyDesc translateDesc(uint32_t desc) {
		//BuddyCoreManager contains a copy of these code
		//Remember changing both.
		MCE_TRACE("Converting desc " << desc);
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

	static uint32_t translateInt(BuddyDesc desc) {

		MCE_TRACE("Converting desc " << desc);
		switch (desc) {
		case NoPath:
			MCE_WARN("DESC_UNKNOWN");
			return DESC_UNKNOWN;
		case Friend:
			return DESC_FRIEND;
		case Apply:
			return DESC_APPLY;
		case Applied:
			return DESC_APPLIED;
		case Block:
			return DESC_BLOCK;
		case Blocked:
			return DESC_BLOCKED;
		case BiBlock:
			return DESC_BLOCK | DESC_BLOCKED;
		default:
			MCE_WARN("default switch. This should not be seen.");
			return DESC_UNKNOWN;
		}
	}

	static uint32_t getMask(BuddyDesc desc) {

		switch (desc) {
		case NoPath:
			MCE_WARN("DESC_UNKNOWN");
			return DESC_MASK;
		case Friend:
			return DESC_MASK_FRIEND;
		case Apply:
		case Applied:
			return DESC_MASK_APPLY;
		case Block:
			return DESC_MASK_BLOCK;
		case Blocked:
			return DESC_MASK_BLOCKED;
		default:
			MCE_WARN("default switch. This should not be seen.");
			return DESC_MASK;
		}
	}
};

class EntryListHolderFactory;

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

class EntryListHolder : public Ice::Object, public IceUtil::Mutex {
public:
	EntryListHolder(uint32_t from, EntryList list);
	EntryListHolder(uint32_t from);
	virtual ~EntryListHolder();

	uint32_t getFrom();

	void add(const Entry& entry);
	void remove(const Entry& key, const Entry& mask);
	void remove(const Entry& key);
	void set(const Entry& key, const Entry& keyMask, const Entry& value,
			const Entry& valueMask);
	void set(const Entry& key,  const Entry& value, const Entry& valueMask);
	EntryList get(const Entry& key); // Retrive only one entry.
	EntryList get(const Entry& key, const Entry& mask); // Retrive entry list.
	void get(const Entry& key, const Entry& mask,
			const EntryListResultBuilderPtr& builder);
	EntryList getAll();
private:
	uint32_t _from;
	EntryList _list;

	//	void dump();
	bool equals(const Entry& l, const Entry& r, const Entry& mask);
	bool equals(const Entry& l, const Entry& r);
	void equalsTrace(const Entry& l, const Entry& r, const Entry& mask);
};
typedef IceUtil::Handle<EntryListHolder> EntryListHolderPtr;

//---------------------------------------------------------------------------

}
}

#endif /*BUDDYENTRY_H_*/
