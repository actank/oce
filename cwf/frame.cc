#include "cwf/frame.h"

#ifdef POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#endif

#include <iostream> // TODO: remove
#include <set>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include "ctemplate/template_dictionary.h"
#include "ctemplate/template.h"
#include "libfcgi/fcgiapp.h"

#include "base/common.h"
#include "base/logging.h"
#include "base/logrotate.h"
#include "base/startuplist.h"
#include "base/ptime.h"
#include "base/pcount.h"
#include "base/asyncall.h"
#include "base/logging.h"

#include "cwf/action.h"
#include "cwf/fooaction.h"
#include "cwf/pattern.h"

namespace xce {
extern cwf::User* Authorize(cwf::Request* q);
}

namespace cwf {

static const std::string kDefaultContentType("text/html; charset=utf-8");

void GenerateCommonHeader(Header* header, HttpStatusCode status_code
        , std::string const & message, std::string const& content_type = "") {
  // HTTP/1.x 200 OK
  header->set_status_code(status_code, message);
  if (!content_type.empty())
    header->Add(HH_CONTENT_TYPE, content_type);
}

bool FrameWork::LoadConfig(const std::string& filename) {
  // TODO: 现在没有想到任何用途，可以删除
  return true;
}

bool FrameWork::InitHost(bool load_action) {
  if (default_actions_)
    host_action_.assign(default_actions_->begin(), default_actions_->end());
  return true;
}

BaseAction* FrameWork::Find(std::string const& url) const {
  for (ActionListType::const_iterator i=host_action_.begin();
    i!= host_action_.end(); ++i) {
      BaseAction* ha = *i;
      if (ha->Match(url))
        return ha;
  }
  return 0;
}

XAR_IMPL(cwferr);
XAR_IMPL(cwfall);
XAR_IMPL(prcGT100);
HttpStatusCode FrameWork::Process(Request* request, Response* response) {
  HttpStatusCode rc;

  xce::ptime pt("FrameWork::Process", false, 100);

  BaseAction* a = Find(request->url());
  if (!a) {
    LOG(INFO) << "Not Found: " << request->url();
    ResponseError(HC_NOT_FOUND, "Not Found", response);
    return HC_NOT_FOUND;
  }

  XAR_INC(cwfall);
  rc = a->Process(request, response);

  if (pt.wall_clock() > 100)
    XAR_INC(prcGT100);

  if (HC_OK != rc) {
    XAR_INC(cwferr);
    ResponseError(rc, "Service", response);
    return rc;
  }
  return rc;
}

FrameWork::ActionListType * FrameWork::default_actions_ = 0;

void FrameWork::RegisterAction(BaseAction* a) {
  if (!default_actions_) {
    default_actions_ = new ActionListType();
  }
  default_actions_->push_back(a);
}

void FrameWork::InitTemplate() {
  // 两个目录 cwd, /data/cwf/tpl/
  
  // TODO: use once
  static bool init_ = false;
  if (!init_) {
    init_ = true;

    ctemplate::Template::SetTemplateRootDirectory("/data/xce/XFeed/etc/feed_view_tpl");
    ctemplate::Template::AddAlternateTemplateRootDirectory("/data/cwf/tpl/");
    ctemplate::Template::AddAlternateTemplateRootDirectory(".");
  }
}

void FrameWork::ResponseError(HttpStatusCode code, const char* message, Response* response) {
  // TODO: 貌似返回非 200 有错误, nginx 不识别？
  GenerateCommonHeader(&response->header(), code, message);
  response->OutputHeader();

  // TODO: status code to template file name
#if 0
  ctemplate::TemplateDictionary error_dict_("error"); // TODO: use member?
  ctemplate::Template* tpl = ctemplate::Template::GetTemplate(
    "404.tpl", ctemplate::STRIP_WHITESPACE);
  
  ASSERT(tpl);
  if (tpl)
    tpl->Expand(response, &error_dict_);
#endif
}

void FastcgiProc(FrameWork* fw) {
  FCGX_Request wrap;
  FCGX_InitRequest(&wrap, 0, 0);

  while (FCGX_Accept_r(&wrap) >= 0) {
    // PTIME(pt, "accept", false, 100);

    Request* q = new Request();
    if (!q->Init(wrap.in, wrap.envp)) {
      LOG(LS_ERROR) << "Request::Init failed.";
    }

    Response* p = new Response(wrap.out, wrap.err);

    HttpStatusCode rc = fw->Process(q, p);
#if 0
    std::string r = p->str();
    FCGX_PutStr(r.c_str(), r.size(), wrap.out);
#endif
    // FCGX_Finish_r(&wrap);

    delete p;
    delete q;
  }
}

class SdkExpand;

int FastcgiMain(int thread_count, const char *log_dir) {
  xce::Start();

  std::string log_file = MakeLogDirPath(log_dir);
  xce::LogRotate::instance().Start(log_file, xce::LS_INFO);

  xce::RunStartupList();

  std::string xar_file = MakeXarFilePath(log_dir);
  LOG(INFO) << "xar_file:" << xar_file;
  xce::xar::instance().set_filename(xar_file);
  xce::xar::start();
 

  FrameWork::InitTemplate();

  // FrameWork::RegisterAction(new EmptyAction());
  // FrameWork::RegisterAction(new TemplateAction());
  // FrameWork::RegisterAction(new xce::FeedTypeAction);

  bool f = ctemplate::Template::StringToTemplateCache("404.tpl", "{{URL}} Not Found");
  ASSERT(f);

  std::auto_ptr<FrameWork> fw(new FrameWork());
  if (!fw->LoadConfig("cwf.conf")) {
    // ugly link hack
    // xce::FeedTypeAction fta;
    // xce::FriendlyTimeModifier ftm;
    LOG(LS_ERROR) << "TODO: config error\n";
    return 1;
  }

  if (!fw->InitHost(true)) {
    LOG(LS_ERROR) << "TODO: InitHost error reason\n";
    return 1;
  }

  int ret = FCGX_Init();
  ASSERT(0 == ret);

  boost::thread_group g;
  for (int i=0; i<thread_count; ++i)
    g.add_thread(new boost::thread(
      boost::bind(&FastcgiProc, fw.get())
    ));

  FastcgiProc(fw.get()); // TODO: 管理 ....
  return 0;
}


std::string MakeLogDirPath(const char* log_dir) {
  size_t len = strlen(log_dir);
  std::ostringstream stem;
  if (log_dir[len - 1] == '/') {
    stem << log_dir << getpid();
  }
  else {
    stem << log_dir << "/" << getpid();
  }
  return stem.str();
}

std::string MakeXarFilePath(const char* log_dir) {
  size_t len = strlen(log_dir);
  std::ostringstream stem;
  if (log_dir[len - 1] == '/') {
    stem << log_dir  << getpid() << ".xar";
  }
  else {
    stem << log_dir << "/" << getpid() << ".xar";
  }
  return stem.str();
}

}
