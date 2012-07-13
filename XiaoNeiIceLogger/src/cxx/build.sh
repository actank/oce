g++ -shared -fPIC -o libXiaoNeiLogger.so.32 XiaoNeiLogger.cpp -I/opt/Ice-3.2/include -I/home/bbc/code/log4cxx/include -I. -L/opt/Ice-3.2/lib -lIce -L/home/bbc/code/log4cxx/lib -llog4cxx
