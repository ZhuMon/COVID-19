from bottle import route, run, request, abort, static_file
import re
import json
import time
import threading
from json2html import *

all_data = []
d_count = 0
win_val_dict = {}
h_or_d = "" # 0 for header, 1 for data
@route("/put_data", method="POST")
def put_data():
    global all_data
    global h_or_d
    global d_count
    
    for k,v in request.POST.allitems():

        for i in v.split("\n\n"):
            if i != '':
                # print(i)
                all_data += [i]
                # print(all_data)
                if i.find("{\"window\"") != -1:
                    h_or_d += "0"
                else:
                    h_or_d += "1"

    
    #d.append(request.forms.get("mmmm"))
    d_count = len(all_data)
    print("put data")
    return 'OK'
   

def organize_data():
    global h_or_d
    global win_val_dict
    global d_count

    while d_count == 0:
        None
    
    local_d_count = 0
    window_name = ""
    data_time = 0
    while True:
        if local_d_count < d_count:
            v = all_data[local_d_count]
            if v == '':
                continue

            # header
            if h_or_d[local_d_count] == "0":
                v = v.replace('\\', '\\\\\\\\')
                j = json.loads(v)
                window_name = j["window"]
                data_time = j["time"]
                
            # data
            else:
                if window_name not in win_val_dict.keys():
                    win_val_dict[window_name] = [{"time": data_time, "data": v}]
                else: 
                    if data_time > win_val_dict[window_name][-1]["time"] + 30:
                        win_val_dict[window_name].append({"time": data_time, "data": v})
                    else:
                        win_val_dict[window_name][-1]["data"] += v
   


            
            local_d_count += 1



def transfer_data(content):
    # find [#B]
    b_start = content.find("[#B]")
    output = str(content)
    while b_start != -1:
        output = output[0:b_start-1] + output[b_start:]
        output = output.replace("[#B]", "", 1)
        b_start = output.find("[#B]")
    return output


@route("/show", method="GET")
def show_data():
    # TODO: only administrator can view
    new_dict = {}
    for k,v in win_val_dict.items():
        if k == '':
            continue
        new_dict[k] = []
        
        for i in v:
            if type(i) == dict:
                tmp = i.copy()
                tmp["time"] = time.asctime(time.localtime(tmp["time"]))
                new_dict[k].append(tmp)

    return json2html.convert(json = new_dict)

@route("/show_decode", method="GET")
def decode_data():
    new_dict = {}
    for k,v in win_val_dict.items():
        if k == '':
            continue
        new_dict[k] = []
        
        for i in v:
            if type(i) == dict:
                tmp = i.copy()
                tmp["time"] = time.asctime(time.localtime(tmp["time"]))
                new_dict[k].append(tmp)
                tmp["data"] = transfer_data(tmp["data"])

    return json2html.convert(json = new_dict)

@route("/get_data", method="GET")
def get_data():
    return str(win_val_dict)

if __name__ == "__main__":
    #run(host="127.0.0.1", port=8081, debug=True)
    t = threading.Thread(target=run, kwargs=dict(host='localhost', port=8081))
    t.start()
    organize_data()
