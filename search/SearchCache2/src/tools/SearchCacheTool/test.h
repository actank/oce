#ifndef _TEST_H_
#define _TEST_H_
//////class SearchCacheTest : public ::testing::Test {
////// protected:
//////   static void SetUpTestCase() {
//////     //user_ids_.push_back(58742);
//////   }
//////   virtual void SetUp() {
//////     user_ids_.push_back(58742);
//////     SearchCacheAdapter::instance().load(58742);
//////     sleep(4);
//////   }
////// public:
//////   //static MyUtil::IntSeq user_ids_;
//////   MyUtil::IntSeq user_ids_;
//////};
//////
//////TEST_F(SearchCacheTest, Remove) {
//////  //1.先确保插入
//////  {
//////    UpdateUnitSeq updateUnits;
//////    UpdateUnit updateUnit;
//////    updateUnit.id = 58742;
//////    updateUnits.push_back(updateUnit);
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(1, results.size());
//////  }
//////  //2.删除
//////  {
//////    UpdateUnitSeq updateUnits;
//////    UpdateUnit updateUnit;
//////    updateUnit.id = 58742;
//////    updateUnit.table = "user_passport";
//////    updateUnit.fieldPairMap["STATUS"] = "4";
//////    updateUnits.push_back(updateUnit);
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(0, results.size());
//////    Int2ByteSeq results2 = SearchCacheAdapter::instance().getSearchCacheMap2(user_ids_);
//////    EXPECT_EQ(0, results.size());
//////  }
//////
//////}
//////
//////TEST_F(SearchCacheTest, UpdateId) {
//////  //1.先确保删除
//////  {
//////    MyUtil::LongSeq uids;
//////    uids.push_back(58742);
//////    SearchCacheAdapter::instance().doRemoveMemCacheData(uids);
//////    sleep(4);
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(0, results.size()) << "先显式地调用SearchCacheAdapter::instance().doRemoveMemCacheData删除，sleep四秒，判断是否删除成功";
//////  }
//////
//////  {
//////    UpdateUnitSeq updateUnits;
//////    UpdateUnit updateUnit;
//////    updateUnit.id = 58742;
//////    updateUnits.push_back(updateUnit);
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(1, results.size()) << "显示调用SearchCacheAdapter::instance().doRemoveMemCacheData 删除之后，调用doUpdate(Id)，sleep四秒， 判断是否成功添加";
//////  }
//////}
//////
//////
//////TEST_F(SearchCacheTest, UpdateFieldMap) {
//////  //1.先确保插入
//////  {
//////    UpdateUnitSeq updateUnits;
//////    UpdateUnit updateUnit;
//////    updateUnit.id = 58742;
//////    updateUnits.push_back(updateUnit);
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(1, results.size());
//////  }
//////
//////  //SearchCacheAdapter::instance().load(58742);
//////  //sleep(4);
//////  {
//////    UpdateUnitSeq updateUnits;
//////    {
//////      UpdateUnit updateUnit;
//////      updateUnit.id = 58742;
//////      updateUnit.table = "user_names";
//////      updateUnit.fieldPairMap["NAME"] = "ZHIGANGWU18";
//////      updateUnits.push_back(updateUnit);
//////    }
//////
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////
//////    Int2ByteSeq results = SearchCacheAdapter::instance().getSearchCacheMap2(user_ids_);
//////    EXPECT_EQ(1, results.size());
//////
//////    SearchMemCacheData data;
//////    data.Unserialize((const char*)(*results.begin()).second.data(), (*results.begin()).second.size());
//////    SearchCacheResult result = data.getProperties();
//////    EXPECT_STREQ("ZHIGANGWU18", result.generels[SEARCH_CACHE_NAME].c_str());
//////  }
//////
//////  {
//////    UpdateUnitSeq updateUnits;
//////    {
//////      UpdateUnit updateUnit;
//////      updateUnit.id = 58742;
//////      updateUnit.table = "user_names";
//////      updateUnit.fieldPairMap["NAME"] = "ZHIGANGWU17";
//////      updateUnits.push_back(updateUnit);
//////      user_ids_.push_back(58742);
//////    }
//////
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////
//////    Int2ByteSeq results = SearchCacheAdapter::instance().getSearchCacheMap2(user_ids_);
//////    EXPECT_EQ(1, results.size());
//////
//////    SearchMemCacheData data;
//////    data.Unserialize((const char*)(*results.begin()).second.data(), (*results.begin()).second.size());
//////    SearchCacheResult result = data.getProperties();
//////    EXPECT_STREQ("ZHIGANGWU17", result.generels[SEARCH_CACHE_NAME].c_str());
//////
//////  }
//////
//////}
//////
//////TEST_F(SearchCacheTest, TestUpdateFieldMapAndgetSearchCacheMap) {
//////  //SearchCacheAdapter::instance().load(58742);
//////  //sleep(4);
//////  {
//////    UpdateUnitSeq updateUnits;
//////
//////    {
//////      UpdateUnit updateUnit;
//////      updateUnit.id = 58742;
//////      updateUnit.table = "user_names";
//////      updateUnit.fieldPairMap["NAME"] = "ZHIGANGWU18";
//////      updateUnits.push_back(updateUnit);
//////      user_ids_.push_back(58742);
//////    }
//////
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(1, results.size());
//////
//////    SearchCacheResult& result = (results.begin())->second;
//////    EXPECT_STREQ("ZHIGANGWU18", result.generels[SEARCH_CACHE_NAME].c_str());
//////  }
//////
//////  {
//////    UpdateUnitSeq updateUnits;
//////    {
//////      UpdateUnit updateUnit;
//////      updateUnit.id = 58742;
//////      updateUnit.table = "user_names";
//////      updateUnit.fieldPairMap["LASTLOGINTIME"] = "2010:04:23:0000";
//////      updateUnits.push_back(updateUnit);
//////      user_ids_.push_back(58742);
//////    }
//////
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(1, results.size());
//////
//////    SearchCacheResult& result = (results.begin())->second;
//////    EXPECT_STREQ("ZHIGANGWU17", result.generels[SEARCH_CACHE_NAME].c_str());
//////  }
//////}
//////
//////TEST_F(SearchCacheTest, UpdateTable) {
//////  //1.先确保插入
//////  {
//////    UpdateUnitSeq updateUnits;
//////    UpdateUnit updateUnit;
//////    updateUnit.id = 58742;
//////    updateUnits.push_back(updateUnit);
//////    SearchCacheAdapter::instance().doUpdate(updateUnits);
//////    sleep(4);
//////    Int2SearchCacheResultMap results = SearchCacheAdapter::instance().getSearchCacheMap(user_ids_);
//////    EXPECT_EQ(1, results.size());
//////  }
//////
//////  // 2. 修改 table 信息
//////  {
//////    Int2ByteSeq results = SearchCacheAdapter::instance().getSearchCacheMap2(user_ids_);
//////    ASSERT_EQ(1, results.size());
//////    SearchMemCacheData data, old_data;
//////    data.Unserialize((const char*)((*results.begin()).second.data()), (*results.begin()).second.size());
//////    old_data.Unserialize((const char*)((*results.begin()).second.data()), (*results.begin()).second.size());
//////    
//////    data._networks.at(0).id = 1;
//////    data._networks.at(0).stage = 1;
//////    data._networks.at(0).name = "1";
//////
//////    BinStringPtr ptr = data.Serialize();
//////    if (ptr) {
//////      BinStr2ByteSeqMap(58742, ptr, results);
//////      SearchCacheAdapter::instance().doUpdateMemCacheData(results);
//////
//////      results = SearchCacheAdapter::instance().getSearchCacheMap2(user_ids_);
//////      ASSERT_EQ(1, results.size());
//////      data.Unserialize((const char*)((*results.begin()).second.data()), (*results.begin()).second.size());
//////      EXPECT_EQ(data._networks.at(0).id, 1);
//////      EXPECT_EQ(data._networks.at(0).stage, 1);
//////      EXPECT_STREQ(data._networks.at(0).name.c_str(), "1");
//////
//////      {
//////        UpdateUnitSeq updateUnits;
//////        UpdateUnit updateUnit;
//////        updateUnit.id = 58742;
//////        updateUnit.table = "network_history";
//////        updateUnits.push_back(updateUnit);
//////        SearchCacheAdapter::instance().doUpdate(updateUnits);
//////        sleep(4);
//////        Int2ByteSeq results = SearchCacheAdapter::instance().getSearchCacheMap2(user_ids_);
//////
//////        ASSERT_EQ(1, results.size());
//////        SearchMemCacheData new_data;
//////        new_data.Unserialize((const char*)((*results.begin()).second.data()), (*results.begin()).second.size());
//////        CompareSearchMemCacheData(old_data, new_data);
//////      }
//////    }
//////  }
//////}
//////

/////*****void CompareSearchMemCacheData(SearchMemCacheData& old_data, SearchMemCacheData& new_data) ;

#endif 
