all:	ran mme hss

ran:	ran.cpp ran.h client.h ue.h utils.h
	g++ ran.cpp -w -o ran client.cpp ue.cpp utils.cpp -lpthread 

mme:	mme.cpp mme.h server.h utils.h
	g++ mme.cpp -w -o mme server.cpp utils.cpp -lpthread 

clean:
	rm -f mme ue hss *~
