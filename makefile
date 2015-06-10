all: mme ue

mme: mme.cpp
	g++ mme.cpp -w -o mme -lpthread

ue: client_ue.cpp client_ue.h
	g++ client_ue.cpp -w -o ue -lpthread 

clean:
	rm -f mme ue *~
