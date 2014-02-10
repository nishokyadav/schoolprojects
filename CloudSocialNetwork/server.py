#!/usr/bin/env python

import sys
import socket
import string
import thread
import time
import SocketServer

logfile = []
logfilename = 'activity.log'

def logsend(string):
  logfile.write(str(time.time()) + ' sent: ' + str(string))

def logrecv(string):
  logfile.write(str(time.time()) + ' recv: ' + str(string) + '\n')

BUFFER = 1024
SERVER_IP = "127.0.0.1"
SERVER_PORT = 5000

# represent a user internally
class User:
  def __init__(self, id, ip, port):
    self.id = id
    self.ip = ip
    self.port = port
  # output the user's information
  def __str__(self):
    return '[id=' + self.id +', ip=' + self.ip + ', port=' + self.port + ']'
  def __repr__(self):
    return self.__str__()

# a threaded function to send pings and check for pongs
def ping(user_id, user_ip, user_port, server_port):
  global logfile
  print "pinging ", user_id

  tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
  
  start = time.time()
  now = time.time()

  has_responded = False;

  # for 1 min
  while now - start < 60:
    try:
      tcp_sock.connect((user_ip, int(user_port)))
      tcp_sock.send('PING ' + user_id + ' ' + user_ip + ' ' + str(user_port) + '\n')
      logsend('PING ' + user_id + ' ' + user_ip + ' ' + str(user_port) + '\n')
      data = tcp_sock.recv(BUFFER)
      if data:
        logrecv(data)
        data_values = data.rstrip('\n').split(' ')
        if data_values[0] == 'PONG' and data_values[1] == user_id and data_values[2] == user_ip and data_values[3] == user_port:
          has_responded = True
    except socket.error:  
      pass
    now = time.time()

  if not has_responded:
    del users[user_id]
  
  tcp_sock.close()

# handle all incoming UDP requests
class UDPRequestHandler(SocketServer.BaseRequestHandler):
    def handle(self):
      global logfile
      data = self.request[0].strip()
      sock = self.request[1]

      data_values = data.rstrip('\n').split(' ')

      logrecv(data)

      command = data_values[0]

      if command == 'REGISTER':
        new_user = User(data_values[1], data_values[2], data_values[3])
        users[new_user.id] = new_user
        sock.sendto('ACK ' + new_user.id + ' ' + new_user.ip + ' ' + new_user.port + '\n', self.client_address)
        logsend('ACK ' + new_user.id + ' ' + new_user.ip + ' ' + new_user.port + '\n')

      elif command == 'QUERY':
        user_name = data_values[1]
        if user_name in users:
          sock.sendto('LOCATION ' + users[user_name].id + ' ' + users[user_name].ip + ' ' + users[user_name].port + '\n', self.client_address)
          logsend('LOCATION ' + users[user_name].id + ' ' + users[user_name].ip + ' ' + users[user_name].port + '\n')
        else:
          sock.sendto('LOCATION ' + user_name + ' 0.0.0.0 0' + '\n', self.client_address)
          logsend('LOCATION ' + user_name + ' 0.0.0.0 0' + '\n')

      elif command == 'QUIT':
        user_name = data_values[1]
        if users[user_name].ip == data_values[2] and users[user_name].port == data_values[3]:
          del users[user_name]

      elif command == 'DOWN':
        user_name = data_values[1]
        try:
          thread.start_new_thread(ping, (user_name, data_values[2], data_values[3], SERVER_PORT))
        except:
          print "Error: can't create a thread."
      print users


users = {}

def main():
  global logfilename
  global logfile

  if len(sys.argv) is 2:
    SERVER_PORT = int(sys.argv[1])
  elif len(sys.argv) is 3:
    SERVER_PORT = int(sys.argv[1])
    logfilename = str(sys.argv[2])
  else:
    exit("Wrong arguments provided.")

  logfile = open(logfilename, 'w')

  # starting listening for UDP requests
  server = SocketServer.UDPServer((SERVER_IP, SERVER_PORT), UDPRequestHandler)
  server.serve_forever()

  logfile.close()

if __name__ == "__main__":
    main()
