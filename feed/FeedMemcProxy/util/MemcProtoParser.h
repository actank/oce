#ifndef _MEMCPROTOPARSER_H_
#define _MEMCPROTOPARSER_H_

#include <string>
using namespace std;
enum CmdType{
  CMD_GET=1, 
  CMD_SET=2, 
  CMD_DELETE=3, 
  CMD_APPEND=4, 
  CMD_VALUE=5, 

  CMD_END=11, 
  CMD_STORED=12, 
  CMD_NOT_STORED=13, 
  CMD_NOT_FOUND=14,
  CMD_DELETED=15, 
  CMD_NOTDEF=-1
};

enum ParserErrCode{
  PARSER_ERR = -1,
  PARSER_SUC = 0,
  PARSER_MORE = 1
};

struct MemCmdHead{
  ParserErrCode succ_;
  CmdType type_;
  int     data_len_;
  char*   body_;
  int     body_len_;
  char*   key_;
  int     key_len_;
  int     flag_;
};
class MemcProtoParser{
public:
  static const char* END_BUF;
  static const int   END_BUF_LEN;
  static const char* STORED_BUF;
  static const int   STORED_BUF_LEN;
  static const char* NOT_STORED_BUF;
  static const int   NOT_STORED_BUF_LEN;
  static const char* DELETED_BUF;
  static const int   DELETED_BUF_LEN;
  static const char* NOT_FOUND_BUF;
  static const int   NOT_FOUND_BUF_LEN;

  static CmdType    GetCmdType(char* buf, int buf_len);
  static int        CmdKeySize(char* buf, int buf_len);
  static MemCmdHead ParserHead(char* buf, int buf_len);
};
#endif
