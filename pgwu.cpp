#include "pgwu.h"

TunUdata::TunUdata(){

	// Dummy
};

TunUdata::~TunUdata(){

	// Dummy
}

PGWu::PGWu(){

	RawSocket::set_interface("lo");
}

void PGWu::recv_sgw(Server &pgw_server){

	pgw_server.read_data();
}

PGWu::~PGWu(){

	// Dummy
}