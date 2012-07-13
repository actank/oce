// **********************************************************************
//
// Copyright (c) 2005-2006 DuDu, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef __ICE_AMD_TASK_H__
#define __ICE_AMD_TASK_H__

#include "TaskManager.h"

namespace IceUtil {
typedef IceUtil::Handle<IceUtil::Shared> SharedPtr;
typedef IceUtil::Handle<IceInternal::GCShared> GCSharedPtr;
}
;

namespace MyUtil {

template<class ParentPtr, class CallbackPtr> class IceAmdTask : public Task {
public:
	IceAmdTask(ParentPtr& parent, CallbackPtr& callback) :
		_parent(parent), _callback(callback) {
		;
	}

	virtual void handle() {
		try {
			execute();
			response();
		} catch (Ice::Exception& e) {
			exception(e);
		} catch (...) {
			exception(Ice::Exception(__FILE__, __LINE__));
		}
	}

	virtual void exception(const Ice::Exception& e) {
		_callback->ice_exception(e);
	}
	virtual void execute() = 0;
	virtual void response() = 0;
protected:
	CallbackPtr _callback;
	ParentPtr _parent;
};

}
;
#endif

