all:
	gcc6 echo_server.cpp -lstdc++
	gcc6 echo_client.cpp -o b.out -lstdc++
	./a.out