#include "socialgraph/socialgraphutil/calculator/component/readfriendlistfromdisk.h"

#include <boost/algorithm/string.hpp>

#include <QueryRunner.h>
#include "socialgraph/socialgraphutil/algostuff.h"

namespace xce {
namespace socialgraph {

ReadFriendListFromDisk::ReadFriendListFromDisk(const std::string path) {
	com::xiaonei::xce::ConnectionPoolManager::instance().initialize();

	std::string dir = boost::ends_with(path, "/") ? path + "friend_data" : path + "/friend_data";
	for (int i = 0; i < INDEX_SIZE; ++i) {
		pathlist_.push_back(dir + "_" + boost::lexical_cast<std::string>(i));
	}
	id2offset_.resize(INDEX_SIZE);
	remain_ = INDEX_SIZE;
	buff_ = new char[5<<20];
}

ReadFriendListFromDisk::~ReadFriendListFromDisk() {
	if (buff_) {
		delete buff_;
	}

	for (std::vector<FILE*>::iterator iter = fplist_.begin();
			iter != fplist_.end(); ++iter) {
		fclose(*iter);
	}
}

bool ReadFriendListFromDisk::LoadFriendListIndex() {
	for (std::vector<std::string>::iterator iter = pathlist_.begin();
			iter != pathlist_.end(); ++iter) {
		FILE* fp = fopen(iter->c_str(), "r+");
		if (NULL == fp) {
			return false;
		}
		fplist_.push_back(fp);
	}

	for (int index = 0; index < INDEX_SIZE; ++index) {
		int position = 0;
		while (true) {
			FriendListHead head;
			if (fread(&head, sizeof(FriendListHead), 1, fplist_.at(index)) != 1) {
				break;
			}
			if (fread(buff_, head.len, 1, fplist_.at(index)) != 1) {
				break;
			}

			id2offset_.at(index).insert(std::make_pair<int, int>(head.uid, position));
			position += sizeof(FriendListHead) + head.len;
		}
	}
	return true;
}

void ReadFriendListFromDisk::LoadData() {
	if (LoadFriendListIndex()) {
		return;
	}

	for (std::vector<std::string>::iterator iter = pathlist_.begin();
			iter != pathlist_.end(); ++iter) {
		FILE* fp = fopen(iter->c_str(), "w+");
		fplist_.push_back(fp);
	}

	std::vector<IceUtil::ThreadPtr> preloadvec;
	for (int i = 0; i < INDEX_SIZE; ++i) {
		preloadvec.push_back(new PreloadThread(this, i));
		preloadvec[i]->start(1024 * 1024).detach();
	}

	while (true) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		if (remain_ > 0) {
			mutex_.wait();
		} else {
			break;
		}
	}

	std::cout << "finish preload" << std::endl;
}

void ReadFriendListFromDisk::GetFriendList(int userid, std::vector<int>& friendlist) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);

	int index = (userid % 100) / 10;  //定位，因为100个表被散到10个索引文件中
	if (NULL == fplist_.at(index)) {
		return;
	}
	__gnu_cxx::hash_map<int, int>::iterator fit = id2offset_.at(index).find(userid);
	if (fit == id2offset_.at(index).end()) {
		return;
	}

	fseek(fplist_.at(index), fit->second, SEEK_SET);
	FriendListHead head;
	fread(&head, sizeof(FriendListHead), 1, fplist_.at(index));
	fread(buff_, head.len, 1, fplist_.at(index));
	
	Frombuff(head.len, buff_, friendlist);
}

void ReadFriendListFromDisk::LoadFriendList(int mod, const std::map<int, std::vector<int> >& friendlist) {
	int index = mod % 10;
	for (std::map<int, std::vector<int> >::const_iterator iter = friendlist.begin();
			iter != friendlist.end(); ++iter) {
		int len = Tobuff(iter->first, iter->second, buff_);
		FriendListHead head;
		head.uid = iter->first;
		head.len = len;

		int size = ftell(fplist_.at(index));
		id2offset_.at(index).insert(std::make_pair<int, int>(head.uid, size));

		fwrite(&head, sizeof(FriendListHead), 1, fplist_.at(index));
		fwrite(buff_, head.len, 1, fplist_.at(index));
	}
}

void ReadFriendListFromDisk::DecreaseRemain() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	remain_--;
	mutex_.notify();
}

int ReadFriendListFromDisk::Tobuff(int id, const std::vector<int>& friendlist, char* buff) const {
  char* p = buff;

  // friendlist
  for (std::vector<int>::const_iterator iter = friendlist.begin();
      iter != friendlist.end(); ++iter) {
    *(int*)p = *iter; p += sizeof(int);
  }
  return p - buff;
}

void ReadFriendListFromDisk::Frombuff(int len, char* buff, std::vector<int>& friendlist) {
  char* p = buff;
  for (int i = 0; i < len; i += sizeof(int)) {
    int fid = *(int*)p; p += sizeof(int);
    friendlist.push_back(fid);
  }
}

void ReadFriendListFromDisk::PreloadThread::run() {
	std::cout << "[ReadFriendListFromDisk] run mod_ : " << mod_ << std::endl; 
	for (int i = 0; i < 10; ++i) {
		int index = 10 * mod_ + i;	
		int last = 0;
		std::string table_name = TableName::RelationTableName(index);
		std::string pattern = TableName::RelationPattern(index);

		while (true) {
			try {
				size_t query_count = 0;
				std::map<int, std::vector<int> > friendlist;
				com::xiaonei::xce::Statement sql;
				sql << "SELECT host, guest FROM " << table_name << " WHERE host > " 
					<< last << " LIMIT " << PRODUCE_BATCH_SIZE;

				query_count = com::xiaonei::xce::QueryRunner("friendfinder_friend", com::xiaonei::xce::CDbRServer, 
					pattern).query(sql, BatchFriendListHandler(last, friendlist));
				if (0 == query_count) {
					std::cout << "[PreloadThread] index(" << index << ") completed"<< std::endl;
					break;
				}

				std::map<int, std::vector<int> >::iterator fit = friendlist.find(last);
				if (fit != friendlist.end()) {
					fit->second.clear();
				}
				com::xiaonei::xce::Statement ss;
				ss << "SELECT host, guest FROM " << table_name << " WHERE host=" << last << " LIMIT 5000"; 
				query_count = com::xiaonei::xce::QueryRunner("friendfinder_friend", com::xiaonei::xce::CDbRServer,
					pattern).query(ss, BatchFriendListHandler(last, friendlist));

				rf_->LoadFriendList(mod_, friendlist);
			} catch (const Ice::Exception& e) {
				std::cout << "[PreloadThread] run ice exception " << e.what() << std::endl;
			} catch (const std::exception& e) {
				std::cout << "[PreloadThread] run std exception " << e.what() << std::endl;
			} catch (...) {
				std::cout << "[PreloadThread] unknown exception" << std::endl;
			}
		}
	}
	rf_->DecreaseRemain();
}

//-------------------------------------------------------------------

bool BatchFriendListHandler::handle(mysqlpp::Row& row) const {
	int host = (int)row["host"];
	last_ = last_ > host ? last_ : host;

	int guest = (int)row["guest"];
	friendlist_[host].push_back(guest);
	return true;
}

}
}
