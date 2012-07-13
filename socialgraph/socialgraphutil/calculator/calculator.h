#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#include <IceUtil/IceUtil.h>

#include "socialgraph/socialgraphutil/calculator/util/ctimecost.h"
#include "socialgraph/socialgraphutil/calculator/component.h"
#include "socialgraph/socialgraphutil/algostuff.h"

namespace xce {
namespace socialgraph {

static const int SIZE = 7;

class Calculator {
public:
	static Component *component_ptr_list[SIZE];

	Calculator();
	virtual ~Calculator(); 

	void Initialize(); 
	void Calc() {
		//全部计算时间统计
		SGCTIME(t, "Calc");

		//初始化
		Initialize(); 

		if (Hook()) {
			Detail();
		}
	}

	//设置组件
	void SetComponent(int slot, Component* component_ptr); 

	//组件加载
	void LoadComponent(int slot); 

	//获取组件的数据
	static Component* Get(int slot); 

	//钩子
	virtual bool Hook() {
		return true;
	}
	//具体业务实现
	virtual void Detail() = 0;

private:
	char* time2str(const time_t ltime, char* str);	
};

}
}

#endif
