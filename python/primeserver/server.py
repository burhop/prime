#!/user/bin/pthon
# Simple Server for sharing out prime numbers
# Mark Burhop 6/14/2020
# Updated 10/27/2021 to access prime server
import sys
import socket
import urllib.request
import certifi
import ssl
if __name__ == "__main__":
    from analysis import isPrime
else:
    from .analysis import isPrime
#from .primeanalysis.analysis import isPrime
#

def startServer(remoteHost, port, password):
    hostname=''
    ipAddressInternal=''
    ipAddressExternal=''
    #Development server
    hostname = socket.gethostname()    
    ipAddressInternal = socket.gethostbyname(hostname)  
    #remoteHost=argv[2]  
    #port=argv[3]
    #password = argv[4]   
    if remoteHost.find("localhost") or remoteHost.find("127.0.0.1"):
        ipAddressExternal="127.0.0.1"
    else:
        ipAddressExternal = urllib.request.urlopen('https://ident.me',context=ssl.create_default_context(cafile=certifi.where())).read().decode('utf8')
    
    server=remoteHost+"register/"+ipAddressExternal+"/"+str(port)+"/"+password
    print("Your Computer Name is: " + hostname)    
    print("Your Local Computer IP Address is: " + ipAddressInternal)  
    print("Your Public Computer IP Address is: " + ipAddressExternal)
    print("The port to be used is "+ port)
    print("starting...")
    s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    s.bind((ipAddressInternal,int(port)))
    s.listen(5)  

    fp = urllib.request.urlopen(server,context=ssl.create_default_context(cafile=certifi.where()))
    mybytes = fp.read()
    print("server said:  "+ str(mybytes))

    while True:
        clientsocket, address = s.accept()
        print(f"Connection from {address} has been established!")
        data=clientsocket.recv(1024)
        print(int(data))
        b=isPrime(int(data))
        clientsocket.send(bytes(str(b), "utf-8"))

if __name__ == "__main__":
    if len(sys.argv)==4:
        #production
        startServer("https://burhop.pythonanywhere.com/register/","12121","XXXXXX")
    else:   
        #development
        #startServer(sys.arg[1:])
        startServer("http://127.0.0.1:5000/","12121","XXXXXX")
