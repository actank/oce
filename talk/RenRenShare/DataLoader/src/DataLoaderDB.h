#ifndef _DATALOADERDB_H_
#define _DATALOADERDB_H_

#include <vector>
#include <Ice/Ice.h>
#include "HotShareInfo.h"
#include "OnlineVideoInfo.h"

namespace xce {
namespace dataloader {

const size_t kHotShareInsertSize = 1000;
const size_t kVideoType = 1;
const size_t kAlbumType = 2;
const size_t kBlogType = 3;

class DataLoaderDB {
  public:
    static bool GetHotShare(const size_t type, Ice::Long &max_db_id, size_t size,  std::vector<HotShareInfo> &hotshare_vector);
    static bool GetOnlineVideo(Ice::Long &max_db_id, size_t size, std::vector<OnlineVideoInfo> &online_video_vector);
    static bool InsertHotShare(std::vector<HotShareInfo> &hotshare_vector, size_t limit_size=0);
    static bool InsertHotSharePart(std::vector<HotShareInfo> &hotshare_vector, size_t begin, size_t insert_size);
    static bool DeleteHotShare(const Ice::Long db_id=-1);
};

}
}
#endif
