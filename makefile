all:	ue mme hss

mme:	mme.cpp
	g++ mme.cpp -w -o mme -lpthread

ue:	client_ue.cpp client_ue.h
	g++ client_ue.cpp -w -o ue -lpthread 

hss:	hss.cpp hss.h
	g++ hss.cpp -o hss `mysql_config --cflags --libs`

clean:
	rm -f mme ue *~
