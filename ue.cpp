#include "ue.h"

UE::UE(int ue_num){
	ue.key = key_generation(ue_num);
	ue.IMSI = 1234567812345678 + ue.key;
	ue.MSISDN = 9000000000 + ue.key;
}

unsigned long long int UE::key_generation(int ue_num){
	return ue_num;
}

UE::~UE(){
	// Dummy destructor
}
