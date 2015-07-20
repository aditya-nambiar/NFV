all:	ran mme hss sgw pgw

G++ = g++ -std=c++0x -std=gnu++0x

RAN_P = ran.cpp ran.h ue.cpp ue.h client.cpp client.h packet.cpp packet.h utils.cpp utils.h

MME_P = mme.cpp mme.h client.cpp client.h server.cpp server.h packet.cpp packet.h utils.cpp utils.h

HSS_P = hss.cpp hss.h db_mysql.cpp db_mysql.h server.cpp server.h packet.cpp packet.h utils.cpp utils.h

SGW_P = sgw.cpp sgw.h client.cpp client.h server.cpp server.h packet.cpp packet.h utils.cpp utils.h

PGW_P = pgw.cpp pgw.h raw_socket.cpp raw_socket.h server.cpp server.h packet.cpp packet.h utils.cpp utils.h

RAN_R = $(G++) ran.cpp -w -o ran ue.cpp client.cpp packet.cpp utils.cpp -lpthread 

MME_R = $(G++) mme.cpp -w -o mme client.cpp server.cpp packet.cpp utils.cpp -lpthread 

HSS_R = $(G++) hss.cpp -w -o hss db_mysql.cpp server.cpp packet.cpp utils.cpp `mysql_config --cflags --libs`

SGW_R = $(G++) sgw.cpp -w -o sgw client.cpp server.cpp packet.cpp utils.cpp -lpthread 

PGW_R = $(G++) pgw.cpp -w -o pgw raw_socket.cpp server.cpp packet.cpp utils.cpp -lpthread 

ran:	$(RAN_P)
	$(RAN_R)

mme:	$(MME_P)
	$(MME_R)
	
hss:	$(HSS_P)
	$(HSS_R)

sgw:	$(SGW_P)
	$(SGW_R)

pgw:	$(PGW_P)
	$(PGW_R)

clean:
	rm -f ran mme hss sgw pgw *~
