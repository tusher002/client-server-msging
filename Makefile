all:
	gcc6 -pthread echo_server.cpp -o b.out -lstdc++
	gcc6 -pthread echo_client_thread.cpp -o c.out -lstdc++
	./b.out