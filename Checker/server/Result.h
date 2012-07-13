/**
 * @file
 * @author  yang.yu@opi-corp.com
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Result.h： enum CheckResult: 检测结果
 *             class Result: 用于保存检测结果以及出错原因
 */

#ifndef __RESULT_H__
#define __RESULT_H__

namespace xce{
  namespace check{

    typedef enum {
      SUCCESS,
      CALLERROR,
      REPLICAVALUEERROR,
      STEPVALUEERROR,
      SEQUENCEERROR
    } CheckResult;


    class Result {
      public:
        CheckResult type;
        std::string content;
      public:
        Result():type(SUCCESS),content(""){
        }

        friend std::ostream& operator<<(std::ostream& os, const Result& result) {
          switch(result.type){
            case SUCCESS:{
                           os << " SUCCESS " << result.content;
                         }break;
            case CALLERROR:{
                             os << " CALLERROR " << result.content; 
                           }break;
            case REPLICAVALUEERROR:{
                                     os << " REPLICAVALUEERROR " << result.content;
                                   }break;
            case STEPVALUEERROR:{
                                  os << " STEPVALUEERROR " << result.content;
                                }break;
            case SEQUENCEERROR:{
                                 os << " SEQUENCEERROR " << result.content;
                               }break;
            default:
                               break;
          }
          return os;
        }
    };

  };
};

#endif
