#include "thread_pool.h"

ThreadPool::ThreadPool(){

	conn_lock = PTHREAD_MUTEX_INITIALIZER;
	conn_req = PTHREAD_COND_INITIALIZER;
	conn_full = PTHREAD_COND_INITIALIZER;
	clear_connections();
}

void ThreadPool::clear_connections(){

	while(!connections.empty())
		connections.pop();	
}

void ThreadPool::set_max_threads(int count){

	max_threads = count; 
	thread_num.resize(max_threads);
	thread_id.resize(max_threads);	
}

void ThreadPool::set_thread_func(void*(*thread_func)(void*)){
	
	this->thread_func = thread_func;
}

void ThreadPool::create_threads(){
	int i;
	int status;

	for(i=0;i<max_threads;i++){
		status = pthread_create(&thread_id[i], NULL, thread_handler, this);
		report_error(status, "Error in creating threads");
	}
}

ClientDetails ThreadPool::fetch_connection(){
	ClientDetails entity;

	if(!connections.empty()){
		entity = connections.front();
		connections.pop();
	}
	else
		entity.num = -1;
	return entity;
}

ThreadPool::~ThreadPool(){

	
}

void* thread_handler(void *arg){
	struct ThreadPool *tpool;
	ClientDetails entity;
	int num;
	int status;
	void *conn_info;

	tpool = ((struct ThreadPool*)arg);
	while(1){
		status = pthread_mutex_lock(&(tpool->conn_lock));
		report_error(status, "Error in locking");
		cout<<"Thread waiting here"<<endl;
		while((entity = tpool->fetch_connection()).num == -1){
			status = pthread_cond_wait(&(tpool->conn_req), &(tpool->conn_lock));
			report_error(status, "Conditional wait failed");
		}
		status = pthread_cond_signal(&(tpool->conn_full));
		report_error(status, "Error in signalling event");
		status = pthread_mutex_unlock(&(tpool->conn_lock));
		conn_info = &entity;
		(*(tpool->thread_func))(conn_info);
	}	
}
