/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintParam.h
 *
 *    Description:  Parameter for HomeFootprint
 *
 *        Version:  1.0
 *        Created:  2011年09月13日 18时21分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __HOME_FOOTPRINT_PARAM__
#define __HOME_FOOTPRINT_PARAM__

namespace xce 
{
namespace footprint
{

const static Ice::Int HOME_FOOTPRINT_REPLICA = 0;

const static std::string COLUMN_OWNER = "owner";
const static std::string DESC_FOOTPRINT = "footprint";

const static std::string COLUMN_VIEWCOUNT = "viewcount";
const static std::string DESC_VIEW_COUNT = "user_view_count";

}
}
#endif
