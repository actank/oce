#ifndef _XMLGENERATOR_H_
#define _XMLGENERATOR_H_

#include <string>
#include "DataProviderInfo.h"

namespace xce {
namespace dataprovider {

const size_t hotshare_data_version = 1;
const size_t string_len  = 32;
const size_t summary_len = 43;

class XMLGenerator {
public:
  static std::string OutputXMLVideo(const HotShareInfo &hotshare_info, const size_t version=hotshare_data_version);
  static std::string OutputXMLPhoto(const PhotoInfo &photo_info, const size_t version=hotshare_data_version);
  static std::string OutputXMLAlbum(const HotShareInfo &hotshare_info,const Ice::Long album_id, const size_t version=hotshare_data_version);

  static std::string OutputXMLPhotoList(const std::vector<std::string> photo_list_vector);
  static std::string OutputXMLHotShare (const std::vector<std::string> &hotshare_xml_vector, const std::vector<Ice::Long> &low_id_vector, 
      const std::vector<Ice::Long> &high_id_vector);

private:
  static std::string XMLEncode(const std::string &source);
  static int UTF8Length(const char *s, int _len);
  static bool ValidCheck(const std::string &xml);
  static const std::string GetCurrentTime();
  static std::string CutUtf8(const std::string &src, size_t limitlen);
  static std::string DeleteInvalidChar(const std::string &src);
};

}
}
#endif
