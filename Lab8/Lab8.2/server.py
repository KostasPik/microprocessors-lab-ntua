from flask import Flask, request

app = Flask(__name__)

@app.route('/data', methods=['POST'])
def data():
    print(request.get_json())
    return 'SOuntris oops', 200

if '__main__' == __name__:
    app.run('0.0.0.0', debug=True)
