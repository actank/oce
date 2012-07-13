#ifndef __OBJECT_CACHE_ICE__
#define __OBJECT_CACHE_ICE__

#include <Util.ice>

module MyUtil
{
    dictionary<long, Object> ObjectMap; 

    class ObjectResult
    {
        ObjectMap data;
    };
    
    class ObjectCache
    {
    	Object findObject(int category, long id);
    	ObjectResult findObjects(int category, MyUtil::LongSeq ids);
    	
    	Object locateObject(int category, long id);
    	ObjectResult locateObjects(int category, MyUtil::LongSeq ids);
    	
    	void addObject(int category, long id, Object o);
    	void addObjects(int category, ObjectMap os);
    	
    	void removeObject(int category, long id);
    	void removeObjects(int category, MyUtil::LongSeq ids);
    	
    	bool hasObject(int category, long id);
    	bool hasObjects(int category, MyUtil::LongSeq ids);
		
		void preloadObject(int category, long id);
		void preloadObjects(int category, MyUtil::LongSeq ids);

		void reloadObject(int category, long id);
		void reloadObjects(int category, MyUtil::LongSeq ids);
		
    };
};

#endif
