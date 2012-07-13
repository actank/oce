#include "socialgraph/blogrecommend/algorithm/method/impl/blogparticiple.h"

#include "base/logging.h"

namespace xce {
namespace socialgraph {

void BlogParticiple::Create() {
	LOG(INFO) << "[BlogParticiple] Create blogid_ : " << blogid_;
}

void BlogParticiple::Get(Items& result) {
	LOG(INFO) << "[BlogParticiple] Get blogid_ : " << blogid_;
}

void BlogParticiple::Display() {
	LOG(INFO) << "[BlogParticiple] Display blogid_ : " << blogid_;
}

}
}
