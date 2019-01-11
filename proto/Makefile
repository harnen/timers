CPP_FLAGS = -std=c++11
LINK_FLAGS = -lndn-cxx -lboost_system

all: producer_thunks consumer_thunks producer_handshake consumer_handshake
	
producer_thunks: producer_thunks.cpp
	g++ -o producer_thunks producer_thunks.cpp $(CPP_FLAGS) $(LINK_FLAGS)

consumer_thunks: consumer_thunks.cpp 
	g++ -o consumer_thunks consumer_thunks.cpp $(CPP_FLAGS) $(LINK_FLAGS)

producer_handshake: producer_handshake.cpp
	g++ -o producer_handshake producer_handshake.cpp $(CPP_FLAGS) $(LINK_FLAGS)

consumer_handshake: consumer_handshake.cpp 
	g++ -o consumer_handshake consumer_handshake.cpp $(CPP_FLAGS) $(LINK_FLAGS)

.PHONY : clean
clean:
	-rm producer_thunks consumer_thunks producer_handshake consumer_handshake

