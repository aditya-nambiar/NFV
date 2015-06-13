#ifndef UTILS_H
#define UTILS_H
#include "utils.h"
#endif

class UE{
	public:
		unsigned long long int key;
		unsigned long long int IMSI;
		unsigned long long int MSISDN;
	
		UE(int);
		unsigned long long int key_generation();
		~UE();		
};
