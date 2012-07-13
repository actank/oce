#ifndef _DATAPROVIDERBUFFER_H
#define _DATAPROVIDERBUFFER_H

#include <string>
#include <vector>
#include <queue>
#include <Ice/Ice.h>
#include "Singleton.h"

namespace xce {
namespace dataprovider {

const size_t kHotShareXMLBufferSize = 10000;
const size_t kPhotoXMLBufferSize = 100000;
class DataProviderBuffer: public MyUtil::Singleton<DataProviderBuffer> {
  public:
    DataProviderBuffer():bottom_id_(0),top_id_(-1),hotshare_xml_vector_(kHotShareXMLBufferSize) {}
    void AddHotShareXML(std::string hotshare_xml);
    void AddAlbumXML(std::string hotshare_xml, Ice::Long album_id);
    bool GetHotShareXML(std::vector<std::string> &hotshare_xml_vector, Ice::Long low_id, Ice::Long high_id,size_t size, Ice::Long &ret_low_id, Ice::Long &ret_high_id);
    bool GetHotShareXML(std::vector<std::string> &hotshare_xml_vector, Ice::Long max_id, size_t size, Ice::Long &ret_low_id, Ice::Long &ret_high_id);
    bool AddPhoto(Ice::Long album_id, std::string photo_xml);
    std::string GetPhoto(Ice::Long album_id);

  private:
    int GetDBIDPos(Ice::Long DBID);

    std::vector<std::string> hotshare_xml_vector_;
    std::map<Ice::Long, std::string> photo_xml_map_;
    std::queue<Ice::Long> album_id_queue_;
    Ice::Long bottom_id_;
    Ice::Long top_id_;
};
}
}

#endif
