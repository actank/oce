#ifndef COMPONENT_H_
#define COMPONENT_H_

namespace xce {
namespace socialgraph {

class Component {
public:
	Component() {}
	virtual ~Component() {}
	virtual void Load() = 0;
};

}
}

#endif
