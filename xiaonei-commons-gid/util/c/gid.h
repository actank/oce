#ifndef _COMMONS_GID_UTIL_H_
#define _COMMONS_GID_UTIL_H_
#include <stdio.h>

#if defined(WIN32) || defined(_MSC_VER)
typedef __int64 int64_t;
#else
#include <stdint.h>
#endif


/* 
* 提供了将原ID转化为统一ID及其相关的算法。
* <p>
* 统一ID的转化算法由架构组和数据中心联合统一提供，如果统一ID的算法尚未包括您的业务类型，请email给 <a
* href="mailto:xn.arch@opi-corp.com">架构组</a>和<a
* href="mailto:xn_db@opi-corp.com">数据中心</a>，他们会为您服务
* 
* @see <a
*      href="http://doc.d.xiaonei.com/index.php?title=%E7%BB%9F%E4%B8%80ID%E7%9A%84%E8%AE%BE%E8%AE%A1">统一ID的设计</a>
* 
* @author 王志亮 zhiliang.wang@opi-corp.com
* @author 张铁蕾 tielei.zhang@opi-corp.com
* @author 刘启荣 qirong.liu@opi-corp.com
* @author 冷昊 hao.leng@opi-corp.com
* @version 统一ID算法1.4
* 
*/

namespace gid {

  enum Type {
    /** 无效 */
    kNull = 0x0,

    /** 个人 */
    kPerson = 0x100,

    /** 公共主页 */
    kPage = 0x110,

    /** 小站 */
    kXiaoZhan = 0x120,

    /** 小组 */
    kXiaoZu = 0x130,

    /** 老群 */
    kGroup = 0x140,

    /** 私群 */
    kMiniGroup = 0x150,

    /** 活动 */
    kEvent = 0x160,

    /** 第三方应用 */
    kApplication = 0x170,

    /** 俱乐部 */
    kOrg = 0x180,

    /** max_obj */
    kMaxObject = 0x300,

    /** 评论 */
    kComment = 0x300,

    /** 状态 */
    kStatus = 0x310,

    /** 日志 */
    kBlog = 0x320,

    /** 照片 */
    kPhoto = 0x330,

    /** 视频 */
    kVedio = 0x340,

    /** 专辑/相册 */
    kAlbum = 0x350,

    /** 分享 */
    kShare = 0x360,

    /** 投票 */
    kVote = 0x370,

    /** 留言 */
    kGossip = 0x380,

    /** 站内消息 */
    kMessage = 0x390,

    /** 请求 */
    kRequest = 0x3A0,

    /** 标签 */
    kTag = 0x3B0,

    /** 地点 */
    kPlace = 0x3C0,

    /** 信息存储中心系统的entry_index条目 */
    kEntryIndex = 0x3D0,

    /** 链接 */
    kLink = 0x3E0,

    /** 音频 */
    kAudio = 0x3F0,

    /** 日志模板(九宫格、凡客体之类的模板) */
    kBlogTemplate = 0x3A1,

     /** max_doc */
    kMaxDocument = 0x3ff,

    // 二级类型(不作为一级类型的)
    kSubtypeSart = 0x7FF000,
  };

class G {
 public:
  static const int64_t kUpperXValueOf =  0xfffffffffff; //最大的原id
 
  // 类型及原Id合成统一Id
  // 返回0标示合成失败,否则返回统一Id
  static int64_t Id(int64_t x, Type type) {
    if (x <= 0 ) {
      return 0;
    }

    if (type == kPerson || type == kPage || type == kOrg ||
        type == kXiaoZhan || type == kMiniGroup) {
      if (IsTypeOf(x, type))
        return x;
      else
        return 0;
    }

    if (x > kUpperXValueOf)
      return 0;
    return ((int64_t)type << 52) | x;
  }

  
  // 提取类型,see http://wiki.rrdba.cn/wiki/index.php/Id_seq
  // 如果参数不正确返回kNull,
  static Type ExtractType(int64_t gid) {
    if (gid <= 0)
      return kNull;

    if ((gid >= 1 && gid <= 599999999) || (gid >= 700000000 && gid <= 2147483647)) {
      return kPerson;
    }
    if (gid >= 670000000 && gid <= 670999999) {
      return kOrg;
    }
    if (gid >= 671000000 && gid <= 672999999) {
      return kXiaoZhan;
    }
    if (gid >= 680000000 && gid <= 680999999) {
      return kMiniGroup;
    }
    if (gid >= 600000000 && gid <= 699999999) {
      return kPage;
    }
    
    if (gid < (1L << 52))
      return kNull;
    
    int64_t value = (gid >> 52);
    return Type(value);
  }

  // 从统一ID中提取原Id
  static int64_t ExtractX(int64_t gid) {
    return gid & kUpperXValueOf;
  }

  // 类型判断,
  // 对主体ID因gid不包含类型信息,因此目前不能判断除kPerson以外的主体ID
  static bool IsTypeOf(int64_t gid, Type type) {
    return type == ExtractType(gid);
  }
};

} // namespace
#endif
