all:a.out 

a.out:main.cpp ./http/http_conn.cpp ./http/http_conn.h ./lock/locker.h ./threadpool/threadpool.h ./timer/lst_timer.h ./timer/lst_timer.cpp ./log/block_queue.h ./log/log.h ./log/log.cpp  ./cgimysql/sql_connection_pool.cpp
	g++ main.cpp ./http/http_conn.cpp ./log/log.cpp ./timer/lst_timer.cpp ./cgimysql/sql_connection_pool.cpp -lpthread -g -lmysqlclient   -o a.out 
clean:
	rm -f a.out