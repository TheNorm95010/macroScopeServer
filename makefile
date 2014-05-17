RPI_LIB = rgbmatrix
CC=g++

IDIR=./include
LDIR=./lib

CXXFLAGS = -Wall -O3 -g -ggdb
LDFLAGS = -L$(LDIR)
LIBS = -lpthread -l$(RPI_LIB)
TARGET = macroScopeServer

SRCS = macroScopeServer.cpp
OBJS = $(SRCS:.cpp=.o)

_INCLUDES = mongoose.c Thread.cpp
INCLUDES = $(patsubst %,$(IDIR)/%,$(_INCLUDES))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

install:
	cp ./macroScopeServer /usr/local/bin
