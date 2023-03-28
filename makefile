all:a.out 

a.out:main.cpp ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h ./threadpool/threadpool.h ./timer/lst_timer.h ./timer/lst_timer.cpp
	g++ main.cpp ./http/http_conn.cpp  ./timer/lst_timer.cpp  -lpthread -g -o a.out 
clean:
	rm -f a.out