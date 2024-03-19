from flask import Flask
backend = Flask(__name__)

@backend.route('/')
def hello_world():
    return 'Hello, World from Flask!'

if __name__ == '__main__':
    backend.run(host='0.0.0.0', port=8080)

