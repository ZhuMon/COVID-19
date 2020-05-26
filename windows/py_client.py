import requests

my_data = {'mmmm': 'I\'m victim'}

r = requests.post('http://0.0.0.0:8080/put_data', my_data)
print(r)
