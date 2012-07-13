#include "DataProviderBuffer.h"
#include "LogWrapper.h"

using namespace MyUtil;
using namespace std;
using namespace xce::dataprovider;

void DataProviderBuffer::AddHotShareXML(std::string hotshare_xml) {
  if (top_id_>0 && (top_id_+1)%kHotShareXMLBufferSize==bottom_id_%kHotShareXMLBufferSize)
    ++bottom_id_;
  hotshare_xml_vector_[(top_id_+1)%kHotShareXMLBufferSize] = hotshare_xml;
  ++top_id_;
  MCE_DEBUG("DataProviderBuffer::AddHotShareXML   top_id_:"<<top_id_<<"    bottom_id_:"<<bottom_id_);
}

void DataProviderBuffer::AddAlbumXML(std::string hotshare_xml, Ice::Long album_id) {
  AddHotShareXML(hotshare_xml);

  album_id_queue_.push(album_id);
  if (album_id_queue_.size() > kPhotoXMLBufferSize) {
    Ice::Long pop_id = album_id_queue_.front();
    album_id_queue_.pop();
    photo_xml_map_.erase(pop_id);
  }
}

bool DataProviderBuffer::GetHotShareXML(std::vector<std::string> &hotshare_xml_vector, Ice::Long low_id, Ice::Long high_id, size_t size,
    Ice::Long &ret_low_id, Ice::Long &ret_high_id) {
  Ice::Long temp_top_id = top_id_;
  Ice::Long temp_bottom_id = bottom_id_;

  if (high_id > temp_top_id)
    high_id = temp_top_id;
  if (size<=0 || low_id>high_id)
    return false;
  if (temp_top_id <= temp_bottom_id)
    return false;
  if (high_id <= temp_bottom_id) {
    ret_high_id = high_id;
    ret_low_id = 0;
    return true;
  }

  Ice::Long i = 0;
  const Ice::Long temp_low_id = low_id<temp_bottom_id ? temp_bottom_id : low_id;
  if (temp_low_id == low_id)
  for (; (size_t)i<size&&high_id-i>=temp_low_id; ++i) {
    hotshare_xml_vector.push_back(hotshare_xml_vector_[(high_id-i)%kHotShareXMLBufferSize]);
  }
  ret_high_id = high_id;
  ret_low_id = high_id - i+1; 
  return true;
}

bool DataProviderBuffer::GetHotShareXML(std::vector<std::string> &hotshare_xml_vector, Ice::Long max_id, size_t size, 
    Ice::Long &ret_low_id, Ice::Long &ret_high_id) {
  Ice::Long temp_top_id = top_id_;
  Ice::Long temp_bottom_id = bottom_id_;
  if (max_id==temp_top_id || size<=0)
    return false;
  if (temp_top_id <= temp_bottom_id)
    return false;
  if (max_id > temp_top_id) {
    ret_high_id = temp_top_id;
    ret_low_id = temp_top_id;
    return true;
  }

  Ice::Long i = 0;
  for (; (size_t)i<size&&temp_top_id-i>max_id&&temp_top_id-i>=temp_bottom_id; ++i) {
    hotshare_xml_vector.push_back(hotshare_xml_vector_[(temp_top_id-i)%kHotShareXMLBufferSize]);
  }
  ret_high_id = temp_top_id;
  ret_low_id = temp_top_id - i+1;
  return true;
}

bool DataProviderBuffer::AddPhoto(Ice::Long album_id, std::string photo_xml) {
  if (album_id < 0)
    return false;
  photo_xml_map_.insert(map<Ice::Long, string>::value_type(album_id, photo_xml));
  MCE_DEBUG("DataProviderBuffer::AddPhoto album_id:"<<album_id<<" photo_xml.length:"<<photo_xml.length());
  return true;
}

string DataProviderBuffer::GetPhoto(Ice::Long album_id) {
  string ret = "";
  if (album_id > 0) {
    map<Ice::Long, string>::iterator iter = photo_xml_map_.find(album_id);
    if (iter != photo_xml_map_.end())
      ret = iter->second;
  }
  return ret;
}
