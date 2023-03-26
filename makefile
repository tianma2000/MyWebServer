all:a.out 

a.out:main.cpp ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h ./threadpool/threadpool.h
	g++ main.cpp ./http/http_conn.cpp  -lpthread -g -o a.out 
clean:
	rm -f a.out