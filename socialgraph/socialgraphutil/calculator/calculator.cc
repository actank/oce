#include "socialgraph/socialgraphutil/calculator/calculator.h"

namespace xce {
namespace socialgraph {

Component *Calculator::component_ptr_list[SIZE];

Calculator::Calculator() {
	//MCE_INIT("./clog", "INFO"); 
}

Calculator::~Calculator() {
	for (int i = 0; i < 7; ++i) {
		if (component_ptr_list[i]) {
			delete component_ptr_list[i];
		}
	}
}

void Calculator::Initialize() {
  time_t now = time(NULL);
  char date_str[20];
  time2str(now, date_str);
	LOG(INFO) << "+---------------------+";
	LOG(INFO) << "|" << date_str << "  |";
	LOG(INFO) << "+---------------------+";
}

void Calculator::SetComponent(int slot, Component* component_ptr) {
	if (slot >= SIZE || slot < 0) {
		return;
	}
	component_ptr_list[slot] = component_ptr;
}

void Calculator::LoadComponent(int slot) {
	if (component_ptr_list[slot]) {
		component_ptr_list[slot]->Load();
	}
}

Component* Calculator::Get(int slot) {
	if (component_ptr_list[slot]) {
		return component_ptr_list[slot];
	}
	return NULL;
}

char* Calculator::time2str(const time_t ltime, char* str) {
  if (NULL == str) {
    return NULL;
  }

  struct tm *Tm = localtime(&ltime);
  sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d",
      Tm->tm_year + 1900,
      Tm->tm_mon + 1,
      Tm->tm_mday,
      Tm->tm_hour,
      Tm->tm_min,
      Tm->tm_sec);
  return str;
}

}
}
