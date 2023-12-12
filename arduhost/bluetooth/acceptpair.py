#!/bin/env python3

import bluetooth
import pdb


pdb.set_trace()
uuid = "94f39d29-7d6d-437d-973b-fba39e49d4ee"

server_sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
server_sock.bind(("",bluetooth.PORT_ANY))
server_sock.listen(1)

port = server_sock.getsockname()[1]

bluetooth.advertise_service( server_sock, "SampleServer", service_id = uuid, 
                             service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
                             profiles=[bluetooth.SERIAL_PORT_PROFILE], 
                             )

print("Waiting for connection on RFCOMM channel %d" % port)

client_sock, client_info = server_sock.accept()
print("Accepted connection from ", client_info)

# Automatically accept pairing 
#client_sock.set_security(bluetooth.BT_SECURITY_LOW)

while True:
  data = client_sock.recv(1024)
  if not data: break
  print("received [%s]" % data)
