all:
	gcc6 server_select.cpp -o s.out -lstdc++
	gcc6 -pthread client_thread.cpp -o c.out -lstdc++