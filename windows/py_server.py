from bottle import route, run, request, abort, static_file

@route("/put_data", method="POST")
def put_data():
    print(request.forms.get("mmmm"))
    return 'OK'
    
@route("/show", method="GET")
def show_data():
    # TODO: only administrator can view
    return "hello world"
    
@route("/get_data", method="GET")
def get_data():
    return "get_data"

if __name__ == "__main__":
    run(host="0.0.0.0", port=8080, debug=True)
