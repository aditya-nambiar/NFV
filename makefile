all:	ue mme hss

ue:	ue.cpp ue.h client.h
	g++ ue.cpp -w -o ue client.cpp

mme:	mme.cpp server.h
	g++ mme.cpp -w -o mme server.cpp -lpthread 

hss:	hss.cpp server.h db.h
	g++ hss.cpp -w -o hss server.cpp db.cpp `mysql_config --cflags --libs`

clean:
	rm -f mme ue hss *~
