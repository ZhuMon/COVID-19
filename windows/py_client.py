import requests

my_data = {'mmmm': 'I\'m victim'}

r = requests.post('http://localhost:8081/put_data', my_data)
print(r)
