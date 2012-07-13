g++ main.cc windows_live_importer.cc pugixml.cpp \
	-I/opt/Ice-3.2/include \
	-I. -I$HOME/code/curl/include -L$HOME/code/curl/lib -lcurl
#	-DVERBOSE_MODE \
