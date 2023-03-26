#ifndef LOCKER_H
#define LOCKER_H

#include <pthread.h>
#include <semaphore.h>
//定义用来互斥的类

//互斥锁
class locker {
public:
	locker(){
		pthread_mutex_init(&m_mutex, NULL);
	}
	~locker() {
		pthread_mutex_destroy(&m_mutex);
	}
	void lock() {
		pthread_mutex_lock(&m_mutex);
	}
	void unlock() {
		pthread_mutex_unlock(&m_mutex);
	}
private:
	pthread_mutex_t m_mutex;
};

//条件变量
class cond {
public:
	cond() {
		pthread_cond_init(&m_cond,NULL);
	}
	~cond() {
		pthread_cond_destroy(&m_cond);
	}
	void wait(pthread_mutex_t * locker) {
		pthread_cond_wait(&m_cond, locker);
	}
	void signal() {
		pthread_cond_signal(&m_cond);
	}
private:
	pthread_cond_t m_cond;
};

//信号量
class sem {
public:
	sem() {
		sem_init(&m_sem, 0, 0);
	}
	sem(int num) {
		sem_init(&m_sem, 0, num);
	}
	~sem() {
		sem_destroy(&m_sem);
	}
	bool wait() {
		sem_wait(&m_sem);
		return true;
	}
	bool post() {
		sem_post(&m_sem);
		return true;
	}
private:
	sem_t m_sem;
};

#endif // !LOCKER_H
