all:	ran mme hss

G++ = g++ -std=c++0x -std=gnu++0x

RAN_P = ran.cpp ran.h client.cpp client.h ue.cpp ue.h utils.cpp utils.h

MME_P = mme.cpp mme.h server.cpp server.h client.cpp client.h utils.cpp utils.h

HSS_P = hss.cpp hss.h server.cpp server.h db_mysql.cpp db_mysql.h utils.cpp utils.h

RAN_R = $(G++) ran.cpp -w -o ran client.cpp ue.cpp utils.cpp -lpthread 

MME_R = $(G++) mme.cpp -w -o mme server.cpp client.cpp utils.cpp -lpthread 

HSS_R = $(G++) hss.cpp -w -o hss server.cpp db_mysql.cpp utils.cpp `mysql_config --cflags --libs`

ran:	$(RAN_P)
	$(RAN_R)

mme:	$(MME_P)
	$(MME_R)
	
hss:	$(HSS_P)
	$(HSS_R)

clean:
	rm -f ran mme hss *~
