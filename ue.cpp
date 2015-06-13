#include "ue.h"

UE::UE(int ue_num){
	key = key_generation(ue_num);
	IMSI = 1234567812345678 + key;
	MSISDN = 9000000000 + key;
}

unsigned long long int UE::key_generation(int ue_num){
	return ue_num;
}

UE::~UE(){
	// Dummy destructor
}
