#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "utils.h"
#include "packet.h"
#include <queue>

struct ThreadPool{
	pthread_mutex_t conn_lock;
	pthread_cond_t conn_req;
	pthread_cond_t conn_full;
	queue<ClientDetails> connections;
	int max_threads;
	vector<int> thread_num;
	vector<pthread_t> thread_id;
	void*(*thread_func)(void*);

	ThreadPool();
	ThreadPool(const ThreadPool&);
	friend void swap(ThreadPool&, ThreadPool&);
	ThreadPool& operator=(ThreadPool);
	ThreadPool(ThreadPool&&);
	void clear_connections();
	void set_max_threads(int);
	void set_thread_func(void*(*thread_func)(void*));
	void create_threads();
	ClientDetails fetch_connection();
	~ThreadPool();
};

void* thread_handler(void*);

#endif //THREAD_POOL_H

