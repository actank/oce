
BASEDIR=${HOME}/code/XiaoNei/trunk/Tour
all:
	slice2cpp $(BASEDIR)/slice/*.ice --output-dir=$(BASEDIR)/generated/cxx
	slice2java $(BASEDIR)/slice/*.ice --output-dir=$(BASEDIR)/generated/java
	g++ -fPIC -I$(BASEDIR)/generated/cxx -I${ICE_HOME}/include -c -g -o Tour.o generated/cxx/Tour.cpp
	g++ -fPIC -I$(BASEDIR)/generated/cxx -I${ICE_HOME}/include -I${BASEDIR}/../IceExt/src -I${BASEDIR}/../build/target -c -g -o TourClient.o src/cxx/TourClient.cpp
	g++ -L${ICE_HOME}/lib -L${BASEDIR}/../release -lIce -lIceUtil -lIceExt -o  TourClient Tour.o TourClient.o
	g++ -fPIC -DMYSQLPP_MYSQL_HEADERS_BURIED -I$(BASEDIR)/generated/cxx -Isrc/cxx -I${ICE_HOME}/include -I${BASEDIR}/../IceExt/src -I${BASEDIR}/../build/target -I${BASEDIR}/../DbCxxPool/src -I${BASEDIR}/../UtilCxx/src -I/usr/local/include -I../buildcmake/tmp/UtilSlice/cpp/ -I../buildcmake/tmp/OceSlice/cpp/ -c -g -o TourI.o src/cxx/TourI.cpp
	g++ --shared -g -o libTour.so Tour.o TourI.o -L/opt/Ice-3.2/lib -L$(BASEDIR)/../release -llog4cplus -lboost_regex -lmysqlpp -lIce -lIceBox -lIceGrid -lIceExt -lUtilSlice -lUtilCxx -lOceSlice -lDbCxxPool -lssl -lmysqlpp
