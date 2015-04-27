__author__ = 'vikram'
import time

import bluetooth


target_name = "HC-05"
target_address = "98:D3:31:30:22:74"

data_sent = False


def recv_timeout(the_socket, timeout=2):
    # make socket non blocking
    the_socket.setblocking(0)

    # total data partwise in an array
    total_data = [];
    data = '';

    #beginning time
    begin = time.time()
    while 1:
        #if you got some data, then break after timeout
        if total_data and time.time() - begin > timeout:
            break

        #if you got no data at all, wait a little longer, twice the timeout
        elif time.time() - begin > timeout * 2:
            break

        #recv something
        try:
            data = the_socket.recv(8192)
            if data:
                total_data.append(data)
                #change the beginning time for measurement
                begin = time.time()
            else:
                #sleep for sometime to indicate a gap
                time.sleep(0.1)
        except:
            pass

    #join all parts to make final string
    return ''.join(total_data)


def send_data(k):
    port = 1
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((target_address, port))
    sock.send(str(k))
    data = recv_timeout(sock)
    print data
    sock.close()


def get_data():
    server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )

    port = 1
    server_sock.bind((target_address,port))
    server_sock.listen(1)
    client_sock,address = server_sock.accept()
    print "Accepted connection from ",address
    data = client_sock.recv(1024)
    print "received [%s]" % data
    client_sock.close()
    server_sock.close()


i = 0

nearby_devices = bluetooth.discover_devices()

for bdaddr in nearby_devices:
    print bdaddr
    if target_name == bluetooth.lookup_name(bdaddr):
        target_address = bdaddr
        break
while True:

    if (target_address is not None) and (not data_sent):
        print "found target bluetooth device with address ", target_address
        k = i % 2
        i += 1
        send_data(k)
        #get_data()
        data_sent = True
    else:
        # print "could not find target bluetooth device nearby"
        # to mark 'connection dropped'
        data_sent = False

    time.sleep(1)



