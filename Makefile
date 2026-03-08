CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall \
-I/opt/homebrew/opt/rocksdb/include

LDFLAGS = \
-L/opt/homebrew/opt/rocksdb/lib \
-L/opt/homebrew/opt/snappy/lib \
-L/opt/homebrew/opt/lz4/lib \
-L/opt/homebrew/opt/zstd/lib \
-L/opt/homebrew/opt/bzip2/lib \
-L/opt/homebrew/opt/zlib/lib

LIBS = -lrocksdb -lsnappy -llz4 -lzstd -lbz2 -lz -pthread

SRC = main.cpp storage_engine.cpp transaction.cpp transaction_manager.cpp worker.cpp lock_manager.cpp
OBJ = $(SRC:.cpp=.o)

all: app

app: $(OBJ)
	$(CXX) $(CXXFLAGS) -o app $(OBJ) $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o app