CFlags = -lws2_32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic

all: server client

server:
	g++ server.cpp -o server.exe $(CFlags)

client:
	g++ klog_main.cpp -o klog_main.exe $(CFlags)

clean:
	rm -f *.exe
