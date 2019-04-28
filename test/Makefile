.PHONY : all test_client test_server clean

CXXFLAGS += -g -Wall -I../include
LDFLAGS += -L../build -lnetwork

all : test_client test_server

test_client : test_client.cpp test_client.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

test_server : test_server.cpp test_server.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

clean :
	-$(RM) test_client test_server
