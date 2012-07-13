
LOG4CXX_DIR=/usr/local/log4cxx

CXX_DIR=src/cxx
CXX_TARGET=libXiaoNeiLogger.so.32
CXX_SRC=$(shell find $(CXX_DIR) -name '*.cpp' -type f)

JAVA_DIR=src/java
JAVA_TARGET=oce-logger.jar
JAVA_SRC=$(shell find $(JAVA_DIR) -name '*.java' -type f)

CXXFLAG=-fPIC \
	-I/opt/Ice-3.2/include \
	-I$(LOG4CXX_DIR)/include
LDFLAG=\
	-L/opt/Ice-3.2/lib -lIce \
	-L$(LOG4CXX_DIR)/lib -llog4cxx
JAVAFLAG=\
	-classpath /opt/Ice-3.2/lib/Ice.jar
ifndef $(JAR)
JAR=jar
endif
ifndef $(JAVAC)
JAVAC=javac
endif

all: cxx java

.PHONY : cxx java clean

cxx: $(CXX_SRC)
	@echo "Compiling C++ sources."
	@$(CC) -shared -o $(CXX_TARGET) $< $(CXXFLAG) $(LDFLAG)

java: $(JAVA_SRC)
	@echo "Compiling Java sources."
	@$(JAVAC) $(JAVAFLAG) $(JAVA_SRC)
	@$(JAR) cf $(JAVA_TARGET) $(JAVA_SRC) $(patsubst %.java,%.class,$(JAVA_SRC))

clean:
	find . -name '*.class' -exec rm -f {} \;
	rm -f $(CXX_TARGET) $(JAVA_TARGET)
