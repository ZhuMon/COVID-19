# usage: 
#     sh change_ip.sh {new_ip}

if [ $# -ne 0 ]; then
   cur_ip="$1"
else
    echo "usage:\n\tsh change_ip.sh {new_ip}"
    return -1
fi

sed -i "s/inet_addr(\".*\"/inet_addr(\"${cur_ip}\"/g" server.cpp
sed -i "s/inet_addr(\".*\"/inet_addr(\"${cur_ip}\"/g" klog_main.cpp
