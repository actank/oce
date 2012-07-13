#include "CardDBAdapter.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"

using namespace xce::cardcache;
using namespace com::xiaonei::xce;
int main(){
  ConnectionPoolManager::instance().initialize();  
  // CardDBAdapter::AddRelation(1001,1021,1,2);
  /*{
    CardInfoSeq cardinfos;
    CardInfoPtr infos[10];
    std::string address = "人";
    for (int i = 0; i < 10; ++i) {
      infos[i] = new CardInfo();
      infos[i]->userId = 112343540 + i;
      infos[i]->address = address;
      address += "人";
      infos[i]->email = "cardemailtest@gamil.com";
      infos[i]->qq = 45312613 + i;
      infos[i]->msn = "cardmsntest@live.com";
      infos[i]->mobile = "12345678910";
      infos[i]->extra = "card附加信息";
      infos[i]->privacy = '1';
      cardinfos.push_back(infos[i]);
    }
    CardDBAdapter::AddInfo(cardinfos);
  }*/

  /*{
    ImportInfoSeq importinfos;
    ImportInfoPtr infos[10];
    std::string address = "大";
    for (int i = 0; i < 10; ++i) {
      infos[i]->userId = 678743540 + i;
      infos[i]->address = address;
      address += "大";
      infos[i]->email = "importemailtest@gamil.com";
      infos[i]->qq = 45312613 + i;
      infos[i]->msn = "importmsntest@live.com";
      infos[i]->mobile = "12345678910";
      infos[i]->extra = "import附加信息";
      infos[i]->importId = 1754937593;
      infos[i]->importWay = 6;
      infos[i]->importDetail = "import具体信息";
      importinfos.push_back(infos[i]);
    }
  }*/
}
