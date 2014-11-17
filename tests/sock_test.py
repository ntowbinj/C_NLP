import socket, struct

sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
#sock.connect(('127.0.0.1', 8181))
sock.connect("/var/run/bays.socket")
s = "that one show on the tv box about the dead walking zombies walkers"
message = "{}{}".format(struct.pack("I", len(s)), s)
try:
    sock.send(message)
    print(sock.recv(100))
except Exception, e:
    print("caught")
    print(e)
finally:
    print("closing")
    sock.close()
