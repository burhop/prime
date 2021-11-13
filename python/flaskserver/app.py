import time
import socket
from socket import error
from flask import Flask, redirect, url_for,render_template
from flask.globals import request
from markupsafe import escape

app = Flask(__name__)
computeServer=''
computeServerPort=''
from config import *
prime=2
@app.route('/')
def index():
    return render_template("index.html", prime=prime)

@app.route('/time')
def get_current_time():
    return {'time': time.time()}

@app.route('/*')
def set_name():
    return 'This was unknown'

@app.route('/register/<location>/<port>/<password>')
def set_computeserver(location,port,password):
    if password!="XXXXXX":
        return "Password not correct."
    global computeServer
    #computeServer=escape(location)
    #this should be an ip. remove any atempts to do otherwise
    computeServer=''.join(i for i in escape(location) if (i.isdigit() or i=="."))
    global computeServerPort
    #computeServerPort=escape(port)
    computeServerPort=''.join(i for i in escape(port) if (i.isdigit()))
    file = open("config.py", "w")
    file.write("computeServer = \""+computeServer +"\"\n")
    file.write("computeServerPort = \""+computeServerPort + "\"\n")
    file.close()
    return 'Registered ' + str(computeServer) + ' at port ' + str(computeServerPort)

@app.route('/lookupnumber', methods=["POST"])
def lookUpNumber():
    number=request.form['num']
    return "Got "+ number

@app.route('/test/<val>', methods=["POST"])
def test(val):
    return "val"

@app.route('/isprime/<number>', methods=["POST"])
def isprime(number):
    if(len(str(computeServer))<7):
        return 'Computer Server : %s on port %s. This will not work. ' % ( str(computeServer),str(computeServerPort))

    s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5)
    try:
        s.connect((str(computeServer),int(computeServerPort)))
    except Exception as ex:
        return "Caught exception socket.error : %s" % str(ex)
    s.send(bytes(number, "utf-8"))
    msgbytes= s.recv(40)
    msg= msgbytes.decode("utf-8")
    return 'Number  %s is %s' % (number, msg)

@app.route('/connect')
def connect():
    if(len(str(computeServer))<7):
        return 'Computer Server : %s on port %s. This will not work. ' % ( str(computeServer),str(computeServerPort))

    s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(5)
    try:
        s.connect((str(computeServer),int(computeServerPort)))
    except Exception as ex:
        return "Caught exception socket.error : %s" % str(ex)

    msgbytes= s.recv(40)
    msg= msgbytes.decode("utf-8")
    s.send(bytes("Got your message. Posting to web page.", "utf-8"))
    return 'Compute Server:  %s :  message %s' % (computeServer, msg)

@app.route('/robot/<location>')
def show_user_profile(location):
    global robot_location
    robot_location= escape(location)
    # show the user profile for that user
    return 'User %s' % escape(location)

if __name__=="__main__":
    app.run()