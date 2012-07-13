SRC_OBJ := mcclient.o zkclient.o
CXXFLAGS += -I.. -I. -I/opt/Ice-3.3/include -g -Wall -O2 -fPIC
libsocialgraphd.so:$(SRC_OBJ)
	echo $(SRC_OBJ)
	g++ -o libsocialgraphd.so -shared $(SRC_OBJ) -L../base -L../account -L../oldslice -L/opt/Ice-3.3/lib -L/usr/local/lib -lbase -laccount -loldsliceslice -lIceUtil -lIceBox -lboost_system -lboost_thread -lboost_program_options -lboost_date_time -ltcmalloc_and_profiler -lzookeeper_mt -lboost_thread -lpthread -Wl,-rpath,/usr/local/lib:/usr/lib64/mysql:/opt/Ice-3.3/lib64
