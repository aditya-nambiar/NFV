all:	ran mme hss

RAN_P = ran.cpp ran.h client.cpp client.h ue.cpp ue.h utils.cpp utils.h

MME_P = mme.cpp mme.h server.cpp server.h utils.cpp utils.h

HSS_P = hss.cpp hss.h server.cpp server.h db.cpp db.h utils.cpp utils.h

RAN_R = g++ ran.cpp -w -o ran client.cpp ue.cpp utils.cpp -lpthread 

MME_R = g++ mme.cpp -w -o mme server.cpp utils.cpp -lpthread 

HSS_R = g++ hss.cpp -w -o hss server.cpp db.cpp utils.cpp `mysql_config --cflags --libs`

ran:	$(RAN_P)
	$(RAN_R)

mme:	$(MME_P)
	$(MME_R)
	
hss:	$(HSS_P)
	$(HSS_R)

clean:
	rm -f ran mme hss *~
