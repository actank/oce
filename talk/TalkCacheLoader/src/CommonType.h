#ifndef __COMMON_TYPE__
#define __COMMON_TYPE__

namespace com
{
namespace xiaonei
{
namespace talk
{
namespace cache
{
namespace loader
{
	enum STATE { NULL_STAT=0, STOP_TOKEN_END=1, WORD_END=2, ENWORD_END=4, NUM_END=8,  MAYBE_BLOCK_WORD_END = 16, BLOCK_WORD_END=32 }; 

    const static int ACTIVE_TYPE_FAST = 1;
	const static int ACTIVE_TYPE_SAME = 2;
	const static int ACTIVE_TYPE_FAST_COUNT_OVER = 4;
	const static int ACTIVE_TYPE_SAME_COUNT_OVER = 8;
	const static int ACTIVE_TYPE_SPAM = 16;
	const static int ACTIVE_TYPE_BLOCK = 32;
	const static int ACTIVE_TYPE_MAYBE_BLOCK = 64;
	const static int ACTIVE_TYPE_AUDIT = 128;
//	const static int ACTIVE_TYPE_SPAM = 128;


//----------------------------------------------

	static int FAST_TIMESLOT = 4;
	static int FAST_OVER_COUNT = 10;
	static int FAST_OVER_TIMESLOT = 120;
	
	static int SAME_TIMESLOT = 50;
	static int SAME_OVER_COUNT = 5;
	static int SAME_OVER_TIMESLOT = 120;
	
	static int MAYBE_OVER_COUNT = 4;
	
	static int FORBIDDEN_BY_BLOCK = 60*60;
	static int FORBIDDEN_BY_MAYBE = 10*60;
	static int FORBIDDEN_BY_SPAM = 10*60;
	static int FORBIDDEN_BY_SAME_OVER = 20*60;
	static int FORBIDDEN_BY_FAST_OVER = 20*60;

	static int RIGHT_CLASS = 0x00000050;       //Class
	static int RIGHT_DESC = 0x00000040;        //涂鸦板
	static int RIGHT_COMMENT = 0x00000020;     //回复（评论）blog
	static int RIGHT_LEAVEWORD = 0x00000010;   //个人页面的留言
	static int RIGHT_ANN = 0x00000008;         //布告、公告
	static int RIGHT_EVENT = 0x00000004;       //活动
	static int RIGHT_MAIL = 0x00000002;        //站内信
	static int RIGHT_POST = 0x00000001;        //群里面的发贴、回复帖子

	static int TRIBE_TOPIC_MSG_TYPE =0;
	static int TRIBE_REPLY_MSG_TYPE =1;
	static int MAIL_MSG_TYPE =2;
	static int CLASS_MSG_TYPE =3;
	static int GOSSIP_MSG_TYPE =4;
	static int TRIBE_NAME_MSG_TYPE = 5;
	static int BLOG_MSG_TYPE = 6;
	static int REPLY_MSG_TYPE = 7;//不做封禁操作
	static int DOODLE_TYPE = 8;
	static int PHOTO_TYPE = 9;	
};
};
};
};
};
#endif
