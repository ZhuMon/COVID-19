from bottle import route, run, request, abort, static_file

d = []
@route("/put_data", method="POST")
def put_data():
    for k,v in request.POST.allitems():
        print(k, v)

    d.append(request.forms.get("mmmm"))
    print("put data")
    return 'OK'
    
@route("/show", method="GET")
def show_data():
    # TODO: only administrator can view
    output_content = ""
    for content in d:
        output_content = f"{output_content}{content}<br>"
        print(content)

    return output_content
    
@route("/get_data", method="GET")
def get_data():
    return "get_data"

if __name__ == "__main__":
    run(host="127.0.0.1", port=8081, debug=True)
