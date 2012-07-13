#ifndef _XCE_CLEAR_SILIVER_WRAP_H_
#define _XCE_CLEAR_SILIVER_WRAP_H_

#include <string>
#include <map>
#include <vector>

#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <ClearSilver/ClearSilver.h>

namespace xce {
namespace notify {

using namespace std;

typedef std::map<std::string, std::string> HdfMap;

class CSTemplateCache;


class CSTemplate {
 public:
  std::string Render(const HdfMap & data);
  static int CheckTpl(const string & tpl, string * err_desc);
  ~CSTemplate();
 private:
  friend class CSTemplateCache;
  CSTemplate() : hdf_(NULL), cs_(NULL) {}

  bool LoadFromString(const char * tpl, int size, const HdfMap & init_vars, string * err_desc = NULL);
  bool LoadFromFile(const char * filename, const HdfMap & init_vars, string * err_desc = NULL);

  static NEOERR* AppendResult(void * res, char * data);
  static NEOERR* MaxLength(CSPARSE *parse, CS_FUNCTION *csf, CSARG *args, CSARG *result);
  static NEOERR* RFeedTime(const char *in, char **ret);

  NEOERR * Init(const HdfMap & init_vars);
  bool ClearHdf();
 private:
  HDF * hdf_;
  CSPARSE * cs_;
  string rendered_;
  // boost::mutex render_mutex_;
  mutable IceUtil::Mutex render_mutex_;
};

class CSTemplateCache {
 protected:
  class TplVector {
   public:
    TplVector(const string & content, const HdfMap & init_vars);
    ~TplVector();
    size_t Size() const { return tpl_vec_.size(); }
    CSTemplate * GetTpl();
   private:
    int index_;
    string tpl_content_;
    vector<CSTemplate *> tpl_vec_;
  };
 public:
  static CSTemplateCache & Instance() {
    static CSTemplateCache instance;
    return instance;
  }
  CSTemplate * GetTemplate(const string & tpl_identifier);
  bool AddTemplate(const string & tpl_identifier, const string & tpl_content, const HdfMap & init_vars);
  bool RemoveTemplate(const string & tpl_identifier);
  virtual ~CSTemplateCache();

  virtual bool Reload() {
    return false;
  }

 protected:
  // map<string, vector<CSTemplate *> > tpl_cache_;
  // map<string, CSTemplate *> tpl_cache_;
  map<string, TplVector*> tpl_cache_;
  mutable IceUtil::RWRecMutex rw_mutex_;
  CSTemplateCache(){}
  CSTemplateCache(const CSTemplateCache &);
};

}
}

#endif // _XCE_CLEAR_SILIVER_WRAP_H_

