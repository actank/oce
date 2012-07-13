#include "test.h"
/////*****void CompareSearchMemCacheData(SearchMemCacheData& old_data, SearchMemCacheData& new_data) {
/////*****  EXPECT_EQ(old_data._id, new_data._id);
/////*****  EXPECT_STREQ(old_data._name.c_str(), new_data._name.c_str());
/////*****  EXPECT_STREQ(old_data._gender.c_str(), new_data._gender.c_str());
/////*****  EXPECT_STREQ(old_data._signature.c_str(), new_data._signature.c_str());
/////*****  //EXPECT_EQ(old_data._viewcount, new_data._viewcount);
/////*****  EXPECT_EQ(old_data._star, new_data._star);
/////*****  EXPECT_EQ(old_data._status, new_data._status);
/////*****  //EXPECT_EQ(old_data._birthday_year, new_data._birthday_year);
/////*****  //EXPECT_EQ(old_data._birthday_month, new_data._birthday_month);
/////*****  //EXPECT_EQ(old_data._birthday_day, new_data._birthday_day);
/////*****  EXPECT_STREQ(old_data._tinyurl.c_str(), new_data._tinyurl.c_str());
/////*****  EXPECT_STREQ(old_data._headurl.c_str(), new_data._headurl.c_str());
/////*****  //EXPECT_STREQ(old_data._astrology.c_str(), new_data._astrology.c_str());
/////*****  EXPECT_EQ(old_data._browse_config, new_data._browse_config);
/////*****  EXPECT_EQ(old_data._status_config, new_data._status_config);
/////*****  //EXPECT_EQ(old_data._basic_config, new_data._basic_config);
/////*****  EXPECT_STREQ(old_data._home_city.c_str(), new_data._home_city.c_str());
/////*****  EXPECT_STREQ(old_data._home_province.c_str(), new_data._home_province.c_str());
/////*****  ///EXPECT_STREQ(old_data._interest.c_str(), new_data._interest.c_str());
/////*****  ///EXPECT_STREQ(old_data._music.c_str(), new_data._music.c_str());
/////*****  ///EXPECT_STREQ(old_data._movie.c_str(), new_data._movie.c_str());
/////*****  ///EXPECT_STREQ(old_data._game.c_str(), new_data._game.c_str());
/////*****  ///EXPECT_STREQ(old_data._comic.c_str(), new_data._comic.c_str());
/////*****  ///EXPECT_STREQ(old_data._sport.c_str(), new_data._sport.c_str());
/////*****  ///EXPECT_STREQ(old_data._book.c_str(), new_data._book.c_str());
/////*****  ///EXPECT_STREQ(old_data._society.c_str(), new_data._society.c_str());
/////*****  //EXPECT_STREQ(old_data._lastlogintime.c_str(), new_data._lastlogintime.c_str());
/////*****  ASSERT_EQ(old_data._networks.size(), new_data._networks.size());
/////*****  for (unsigned int i=0; i< old_data._networks.size(); ++i) { 
/////*****     EXPECT_EQ(old_data._networks[i].id, new_data._networks[i].id);
/////*****     EXPECT_EQ(old_data._networks[i].stage, new_data._networks[i].stage);
/////*****     EXPECT_STREQ(old_data._networks[i].name.c_str(), new_data._networks[i].name.c_str());
/////*****  } 
/////*****
/////*****  ASSERT_EQ(old_data._regions.size(), new_data._regions.size());
/////*****  for (unsigned i=0; i< old_data._regions.size(); ++i) { 
/////*****     EXPECT_EQ(old_data._regions[i].id, new_data._regions[i].id);
/////*****     EXPECT_STREQ(old_data._regions[i].city.c_str(), new_data._regions[i].city.c_str());
/////*****     EXPECT_STREQ(old_data._regions[i].province.c_str(), new_data._regions[i].province.c_str());
/////*****  } 
/////*****
/////*****  ASSERT_EQ(old_data._workplaces.size(), new_data._workplaces.size());
/////*****  for (unsigned i=0; i< old_data._workplaces.size(); ++i) { 
/////*****     EXPECT_EQ(old_data._workplaces[i].id, new_data._workplaces[i].id);
/////*****     EXPECT_STREQ(old_data._workplaces[i].name.c_str(), new_data._workplaces[i].name.c_str());
/////*****  } 
/////*****
/////*****  ASSERT_EQ(old_data._universities.size(), new_data._universities.size());
/////*****  for (unsigned i=0; i< old_data._universities.size(); ++i) { 
/////*****     EXPECT_EQ(old_data._universities[i].id, new_data._universities[i].id);
/////*****     EXPECT_EQ(old_data._universities[i].year, new_data._universities[i].year);
/////*****     EXPECT_STREQ(old_data._universities[i].name.c_str(), new_data._universities[i].name.c_str());
/////*****     EXPECT_STREQ(old_data._universities[i].department.c_str(), new_data._universities[i].department.c_str());
/////*****     EXPECT_STREQ(old_data._universities[i].dorm.c_str(), new_data._universities[i].dorm.c_str());
/////*****  } 
/////*****
/////*****  //TODO:测中小学
/////*****}
/////*****

