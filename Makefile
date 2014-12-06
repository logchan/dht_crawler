CC = g++
CCFLAGS = -Wall -std=c++11
LNKFLAGS = -ltorrent-rasterbar -lboost_system
OUTPUTDIR = ~
OBJECTS = dht_crawler.o main_dht_crawler.o
TARGET = dht_crawler

all:	$(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) $(LNKFLAGS) -o $(OUTPUTDIR)/$(TARGET)
	
dht_crawler.o:
	$(CC) $(CCFLAGS) -c dht_crawler.cpp
	
main_dht_crawler.o:
	$(CC) $(CCFLAGS) -c main_dht_crawler.cpp
	
clean:
	rm $(OBJECTS)
