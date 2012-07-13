#ifndef __RelationI_h__
#define __RelationI_h__

#include "Relation.h"
#include "Date.h"

namespace mop
{

namespace hi
{

namespace svc
{

namespace relation
{
using namespace MyUtil;


//---------------------------------------------------------------------------
class RelationManagerI : virtual public RelationManager
{
public:

    virtual void addRequest(
    				::Ice::Int,
				    ::Ice::Int,
				    const ::MyUtil::Str2StrMap&,
				    const Ice::Current& = Ice::Current());
	virtual void sendRequest(
    				::Ice::Int,
				    ::Ice::Int,
				    const ::MyUtil::Str2StrMap&,
				    const Ice::Current& = Ice::Current());
				    	
    virtual void confirmRequest(
	    			::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());
	virtual void acceptRequest(
					::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());
					
    virtual void rejectRequest(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());
    virtual void denyRequest(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual void rejectAllRequest(
    				::Ice::Int,
				    const Ice::Current& = Ice::Current());
	virtual void denyAllRequest(
    				::Ice::Int,
				    const Ice::Current& = Ice::Current());

    virtual ::Ice::Int getRequestCount(
    				::Ice::Int,
				    const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMapSeq getRequestList(
    				::Ice::Int,
					::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual MyUtil::RowSetPtr getRequestRowSet(
    				Ice::Int,
                    Ice::Int,
                    Ice::Int,
                    const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMap getRequest(
    				::Ice::Int,
				    ::Ice::Int,
				    const Ice::Current& = Ice::Current());
	
	virtual RequestDataSeq getRequestSeq(
					Ice::Int,
                    Ice::Int,
                    Ice::Int,
                    const Ice::Current& = Ice::Current());
                            
    virtual void addBlock(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());
    virtual void block(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual void removeBlock(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());
    virtual void unblock(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual ::Ice::Int getBlockCount(
    				::Ice::Int,
				    const Ice::Current& = Ice::Current());
				    	
	virtual ::MyUtil::Str2StrMap getBlock(
					::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMapSeq getBlockList(
    				::Ice::Int,
					::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual MyUtil::RowSetPtr getBlockRowSet( 
    				Ice::Int,
                    Ice::Int,
                    Ice::Int,
                    const Ice::Current& = Ice::Current());

	virtual BlockDataSeq getBlockSeq(
					Ice::Int sourceId, 
					Ice::Int begin, 
					Ice::Int limit,
					const Ice::Current& = Ice::Current());

    virtual ::Ice::Int getBlockedCount(
    				::Ice::Int,
				    const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMapSeq getBlockedList(
    				::Ice::Int,
					::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual MyUtil::RowSetPtr getBlockedRowSet( 
    				Ice::Int,
                    Ice::Int,
                    Ice::Int,
                    const Ice::Current& = Ice::Current());

    
    virtual void addBuddy(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual void removeBuddy(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual ::Ice::Int getBuddyCount(
    				::Ice::Int,
				    const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMapSeq getBuddyList(
    				::Ice::Int,
					::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

    virtual MyUtil::RowSetPtr getBuddyRowSet(
    				Ice::Int,
                    ::Ice::Int,
                    ::Ice::Int,
                    const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMap getBuddy(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

	virtual BuddyDataSeq getBuddySeq(
					Ice::Int hostId,
					Ice::Int begin,
					Ice::Int limit,
					const Ice::Current& = Ice::Current());
							
    virtual ::Ice::Int getRelationCount(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());

	virtual ::MyUtil::Str2StrMap getRelationA(
                                ::Ice::Int,
                                        ::Ice::Int,
                                        const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMap getRelation(
    				::Ice::Int,
					::Ice::Int,
					const Ice::Current& = Ice::Current());
};

class RelationGroupManagerI : virtual public RelationGroupManager
{
public:

    virtual ::MyUtil::Str2StrMap addRelationGroup(::Ice::Int,
						  const ::std::string&,
						  const Ice::Current& = Ice::Current());

    virtual void removeRelationGroup(::Ice::Int,
				     ::Ice::Int,
				     const Ice::Current& = Ice::Current());

    virtual void renameRelationGroup(::Ice::Int,
				     ::Ice::Int,
				     const ::std::string&,
				     const Ice::Current& = Ice::Current());

    virtual ::Ice::Int getRelationGroupCount(::Ice::Int,
					     const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMapSeq getRelationGroups(::Ice::Int,
						      ::Ice::Int,
						      ::Ice::Int,
						      const Ice::Current& = Ice::Current());

    virtual MyUtil::RowSetPtr getRelationGroupRowSet(Ice::Int,
                                Ice::Int,
                                Ice::Int,
                                const Ice::Current& = Ice::Current());
	virtual GroupDataSeq getRelationGroupSeq(Ice::Int,
                                Ice::Int,
                                Ice::Int,
                                const Ice::Current& = Ice::Current());

    virtual ::MyUtil::Str2StrMap getRelationGroup(::Ice::Int,
						  ::Ice::Int,
						  const Ice::Current& = Ice::Current());

    virtual void moveBuddy(::Ice::Int,
			   ::Ice::Int,
			   ::Ice::Int,
			   const Ice::Current& = Ice::Current());
};

class InternalManagerI : virtual public InternalManager
{
public:

    virtual void reloadRequest(::Ice::Int,
			       const Ice::Current& = Ice::Current());

    virtual void reloadBlock(::Ice::Int,
			     const Ice::Current& = Ice::Current());

    virtual void reloadBlocked(::Ice::Int,
			     const Ice::Current& = Ice::Current());

    virtual void reloadBuddy(::Ice::Int,
			     const Ice::Current& = Ice::Current());

    virtual void reloadRelationGroup(::Ice::Int,
				     const Ice::Current& = Ice::Current());

    virtual void addRequest(::Ice::Int,
			    const ::MyUtil::Str2StrMap&,
			    const Ice::Current& = Ice::Current());

    virtual void addBuddy(::Ice::Int,
			  const ::MyUtil::Str2StrMap&,
			  const Ice::Current& = Ice::Current());

    virtual void addBlock(::Ice::Int,
			  const ::MyUtil::Str2StrMap&,
			  const Ice::Current& = Ice::Current());

    virtual void addBlocked(::Ice::Int,
			    const ::MyUtil::Str2StrMap&,
			    const Ice::Current& = Ice::Current());

    virtual void removeRequest(::Ice::Int,
			       ::Ice::Int,
			       const Ice::Current&);

    virtual void removeBuddy(::Ice::Int,
			     ::Ice::Int,
			     const Ice::Current&);

    virtual void removeBlock(::Ice::Int,
			     ::Ice::Int,
			     const Ice::Current&);

    virtual void removeBlocked(::Ice::Int,
			       ::Ice::Int,
			       const Ice::Current&);

    virtual bool containsRequest(::Ice::Int,
				 ::Ice::Int,
				 const Ice::Current&);

    virtual bool containsBuddy(::Ice::Int,
			       ::Ice::Int,
			       const Ice::Current&);

    virtual bool containsBlock(::Ice::Int,
			       ::Ice::Int,
			       const Ice::Current&);

    virtual bool containsBlocked(::Ice::Int,
				 ::Ice::Int,
				 const Ice::Current&);
};

}

}

}

}

#endif

