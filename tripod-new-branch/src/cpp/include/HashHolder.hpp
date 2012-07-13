#ifndef _TRIPOD_HASHHOLDER_HPP__
#define _TRIPOD_HASHHOLDER_HPP__

#include <string>
#include <map>
#include <boost/thread/shared_mutex.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <vector>
#include <openssl/md5.h>

#include <openssl/crypto.h>

#ifdef CHARSET_EBCDIC
#include <openssl/ebcdic.h>
#endif


#include <TripodData.h>

namespace com {
namespace renren {
namespace tripod {

template<class T>
    class HashHolder {
        public:
            HashHolder(int virtualNodes = 100) : virtualNodes_(virtualNodes) {
            }

        public:
            void renew(const std::map<std::string, T>& config) {
                std::map<uint32_t, std::string> virtualNodesToRealNodesIndex;
                std::map<uint32_t, T> newConfig = hashConfig(config, virtualNodesToRealNodesIndex);
                std::vector<std::pair<uint32_t, T> > newConfigIndex;
                for(typename std::map<uint32_t, T>::const_iterator it = newConfig.begin(); it != newConfig.end(); ++it) {
                    newConfigIndex.push_back(std::pair<uint32_t, T>(it->first, it->second));
                }

                boost::upgrade_lock<boost::shared_mutex> lock(mutex_);
                boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);
                virtualNodesToRealNodesIndex_ = virtualNodesToRealNodesIndex;
                originalConfig_ = config;
                configIndex_.swap(newConfigIndex);
            }

            T get(const std::string& key) {
                std::vector<std::pair<uint32_t, T> > configIndex;
                {
                    boost::shared_lock<boost::shared_mutex> lock(mutex_);
                    configIndex = configIndex_; 
                }

                if(configIndex.size() == 0) {
                    return T();
                }

                return get(key, configIndex);
            }

            std::map<T, KeySeq> get(const KeySeq& keys) {
                std::map<T, KeySeq> res;
                
                std::vector<std::pair<uint32_t, T> > configIndex;
                {
                    boost::shared_lock<boost::shared_mutex> lock(mutex_);
                    configIndex = configIndex_; 
                }

                if(configIndex.size() == 0) {
                    return res;
                }

                for(KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
                    T t = get(*it, configIndex);
                    if(t == NULL) {
                        continue;
                    }
                    typename std::map<T, KeySeq>::iterator findIt = res.find(t);
                    if(findIt == res.end()) {
                        KeySeq newKeys;
                        newKeys.push_back(*it);
                        res.insert(make_pair(t, newKeys));
                    }else{
                        findIt->second.push_back(*it);
                    }
                }
                return res;
            }

            KeySeq get(const KeySeq& keys, const std::vector<std::string>& prxVec) {
                KeySeq res;
                std::map<uint32_t, std::string> virtualNodesToRealNodesIndex;
                
                std::vector<std::pair<uint32_t, T> > configIndex;
                {
                    boost::shared_lock<boost::shared_mutex> lock(mutex_);
                    virtualNodesToRealNodesIndex = virtualNodesToRealNodesIndex_; 
                }

                if(virtualNodesToRealNodesIndex.size() == 0) {
                    return res;
                }
                
                std::set<std::string> prxSet;

                prxSet.insert(prxVec.begin(), prxVec.end());

                if(prxSet.empty()) {
                    return res;
                }
            
                for(KeySeq::const_iterator it = keys.begin(); it != keys.end(); ++it) {
                    typename std::pair<uint32_t, T> findIt = getPair(*it, configIndex);

                    std::map<uint32_t, std::string>::const_iterator findPrxIt = virtualNodesToRealNodesIndex.find(findIt.first);

                    if(findPrxIt != virtualNodesToRealNodesIndex.end()) {
                        if(prxSet.find(findPrxIt->second) != prxSet.end()) {
                            res.push_back(*it);
                        }
                    }
                }

                return res;

            }

            std::map<std::string, T> getOriginalPrxMap() {
                boost::shared_lock<boost::shared_mutex> lock(mutex_);
                return originalConfig_;
            }

        private:
            T get(const std::string& key, const std::vector<std::pair<uint32_t,T> >& configIndex) {
                return getPair(key, configIndex).second;
            }

            std::pair<uint32_t, T> getPair(const std::string& key, const std::vector<std::pair<uint32_t,T> >& configIndex) {
                uint32_t hash = hashKey(key);

                int left = 0;
                int right = configIndex.size() - 1;
                int end = right;
                int middle;
                while(left < right) {
                    middle = left + (right - left)/2;
                    if(configIndex[middle].first < hash) {
                        left = middle + 1;
                        continue;
                    }else if(configIndex[middle].first > hash){
                        right = middle;
                        continue;
                    }else {
                        return configIndex[middle];
                    }
                }

                if(right == end) {
                    right = 0;
                }
               
                return configIndex[right];
            }
           
            std::map<uint32_t, T> hashConfig(const std::map<std::string, T>& config, std::map<uint32_t, std::string>& virtualNodesToRealNodesIndex) {
                std::map<uint32_t, T> res;
                for(typename std::map<std::string, T>::const_iterator it = config.begin(); it != config.end(); ++it) {
                    for(int i = 0; i < virtualNodes_ / 4; ++i) {
                        std::ostringstream os;
                        os<<it->first<<i;

                        std::string key = os.str();

                        unsigned char results[16];
               
                        stringToMD5(key, results);

                        for(int h = 0; h < 4; ++h) {
                            uint32_t hashKey = ((uint32_t) (results[3 + h] & 0xFF) << 24)
                                | ((uint32_t) (results[2 + h] & 0xFF) << 16)
                                | ((uint32_t) (results[1 + h] & 0xFF) << 8)
                                | (results[0 + h] & 0xFF);
                            res.insert(make_pair(hashKey, it->second));
                            virtualNodesToRealNodesIndex.insert(make_pair(hashKey, it->first));
                        }
                    }
                    
                }
                return res;
            }

            uint32_t hashKey(const std::string& key) {
                unsigned char results[16];
                
                stringToMD5(key, results);
                
                return ((uint32_t) (results[3] & 0xFF) << 24)
                  | ((uint32_t) (results[2] & 0xFF) << 16)
                  | ((uint32_t) (results[1] & 0xFF) << 8)
                  | (results[0] & 0xFF);
            }
        protected:

            void stringToMD5(const std::string& source, unsigned char* res) {
                return M_MD5((unsigned char*)source.c_str(), strlen(source.c_str()), res);
            }

            void M_MD5(const unsigned char *d, size_t n, unsigned char *md) {
                MD5_CTX c;
                unsigned char m[MD5_DIGEST_LENGTH];

                if (md == NULL) md=m;

                if (!MD5_Init(&c))
                    return;

                #ifndef CHARSET_EBCDIC
                MD5_Update(&c,d,n);
                #else
                {
                    char temp[1024];
                    unsigned long chunk; 

                    while (n > 0) {
                        chunk = (n > sizeof(temp)) ? sizeof(temp) : n;
                        ebcdic2ascii(temp, d, chunk);
                        MD5_Update(&c,temp,chunk);
                        n -= chunk;
                        d += chunk;
                    }
                }
                #endif
                MD5_Final(md,&c);
                OPENSSL_cleanse(&c,sizeof(c)); /* security consideration */
            }

        private:
            std::vector<std::pair<uint32_t, T> > configIndex_;
            std::map<std::string, T> originalConfig_;
            std::map<uint32_t, std::string> virtualNodesToRealNodesIndex_;
            boost::shared_mutex mutex_;
            int virtualNodes_;
    };

}
}
}
#endif
