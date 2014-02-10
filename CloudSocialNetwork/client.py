#!/usr/bin/env python

import socket
import sys
import thread
import time
import SocketServer
import Queue
import threading
import dropbox

BUFFER = 1024

queue = Queue.Queue()

logfile = []
logfilename = 'activity.log'

# write sending messages to the log
def logsend(string):
  logfile.write(str(time.time()) + ' sent: ' + str(string))

# write receiving messages to the log
def logrecv(string):
  logfile.write(str(time.time()) + ' recv: ' + str(string) + '\n')

# handle user input for friends through "client" socket
def friendHandler(conn, addr):
  global friend
  global friend_ip
  global friend_port
  global friend_msg_count
  global logfile

  # as long as the client has a friend
  while friend != '':
    if not queue.empty():
      user_input = queue.get()
      if user_input[0] == 'CHAT':
        friend_msg_count += 1
        conn.sendall('CHAT ' + str(friend_msg_count) + ' ' + ' '.join(user_input[1:]) + '\n')
        logsend('CHAT ' + str(friend_msg_count) + ' ' + ' '.join(user_input[1:]) + '\n')
      elif user_input[0] == 'TERMINATE':
        conn.sendall('TERMINATE\n')
        logsend('TERMINATE\n')
        friend = ''
        friend_port = 0
        friend_ip = ''
        friend_msg_count = 0
        print "Ending friendship."
      elif user_input[0] == 'REQUEST' or user_input[0] == 'RELAY' or user_input[0] == 'GET':
        logsend(' '.join(user_input))
        conn.sendall(' '.join(user_input))

      else:
        print "Error: invalid command while you are connected to another user."

# handle user input for friends through "server" socket
def friendLocalInputHandler(s):
  global friend
  global friend_ip
  global friend_port
  global friend_msg_count
  global logfile

  while friend != '':
    if not queue.empty():
      user_input = queue.get()
      if user_input[0] == 'CHAT':
        friend_msg_count += 1
        s.sendall('CHAT ' + str(friend_msg_count) + ' ' + ' '.join(user_input[1:]) + '\n')
        logsend('CHAT ' + str(friend_msg_count) + ' ' + ' '.join(user_input[1:]) + '\n')
      elif user_input[0] == 'TERMINATE':
        s.sendall('TERMINATE\n')
        logsend('TERMINATE\n')
        friend = ''
        friend_port = 0
        friend_ip = ''
        friend_msg_count = 0
        print "Ending friendship."
      elif user_input[0] == 'REQUEST' or user_input[0] == 'RELAY' or user_input[0] == 'GET':
        logsend(' '.join(user_input))
        s.sendall(' '.join(user_input))
      else:
        print "Error: invalid command while you are connected to another user."

# handle responding to friend calls
def friendLocalHandler(s):
  global friend
  global friend_ip
  global friend_port
  global friend_msg_count
  global logfile
  while friend != '':
    data = s.recv(1024).strip()
    if data:
      logrecv(data)
      data_values = data.rstrip('\n').split(' ')
      if data_values[0] == 'CHAT':
        print "CHAT ", data_values[1], ": ", ' '.join(data_values[2:])
        friend_msg_count += 1
        s.sendall('DELIVERED ' + str(friend_msg_count) + '\n')
        logsend('DELIVERED ' + str(friend_msg_count) + '\n')
      elif data_values[0] == 'TERMINATE':
        print "Friendship ended."
        friend = ''
        friend_ip = ''
        friend_port = 0
        friend_msg_count = 0
        break
      elif data_values[0] == 'REQUEST':
        f = open('publicProfile.xml', 'r') 
        file_contents = f.read()
        conn.sendall('PROFILE ' + SELF_ID + ' ' + data_values[1] + ' ' + len(file_contents) + ' ' + file_contents)
        logsend('PROFILE ' + SELF_ID + ' ' + data_values[1] + ' ' + len(file_contents) + ' <file contents removed>' )
        f.close()
      elif data_values[0] == 'RELAY':
        f = open(data_values[1] + '.xml', 'r') # very dangerous
        file_contents = f.read()
        conn.sendall('PROFILE ' + data_values[1] + ' ' + data_values[2] + ' ' + len(file_contents) + ' ' + file_contents)
        logsend('PROFILE ' + data_values[1] + ' ' + data_values[2] + ' ' + len(file_contents) + ' <file contents removed>')
        f.close()
      elif data_values[0] == 'GET':
        f = open(data_values[1] + '.xml', 'r') # very dangerous
        file_contents = f.read()
        conn.sendall('FILE ' + data_values[1] + ' ' + len(file_contents) + ' ' + file_contents)
        logsend('FILE ' + data_values[1] + ' ' + data_values[2] + ' ' + len(file_contents) + ' <file contents removed>')
        f.close()

# handle general individual calls
def personHandler(conn, addr):
  global friend
  global friend_ip
  global friend_port
  global friend_msg_count
  global udp_sock
  global logfile
  # self.request is the TCP socket connected to the client
  data = conn.recv(1024).strip()
  logrecv(data)
  print "{} wrote:".format(addr[0])
  print data

  data_values = data.rstrip('\n').split(' ')

  print str(data_values)
  print friend, friend_ip, friend_port

  if data_values[0] == 'PING':
    print "ponging from ", SELF_ID
    conn.sendall('PONG ' + SELF_ID + ' ' + SELF_IP + ' ' + str(SELF_PORT) + '\n')
    logsend('PONG ' + SELF_ID + ' ' + SELF_IP + ' ' + str(SELF_PORT) + '\n')
  elif friend == '' and data_values[0] == 'FRIEND':
    friend = data_values[1]
    friend_ip = addr[0]
    friend_port = int(addr[1])
    conn.sendall('CONFIRM ' + SELF_ID + '\n')
    logsend('CONFIRM ' + SELF_ID + '\n')
    print "Friendship confirmed."

    # manage friendship sending responsibilities
    thread.start_new_thread(friendHandler, (conn, addr))

    # manage friendship receiving responsibilities
    while friend != '':
      data = conn.recv(1024).strip()
      if data:
        logrecv(data)
        data_values = data.rstrip('\n').split(' ')
        if data_values[0] == 'CHAT':
          print "CHAT ", data_values[1], ": ", ' '.join(data_values[2:])
          friend_msg_count += 1
          conn.sendall('DELIVERED ' + str(friend_msg_count) + '\n')
          logsend('DELIVERED ' + str(friend_msg_count) + '\n')
        elif data_values[0] == 'TERMINATE':
          print "Friendship ended."
          friend = ''
          friend_ip = ''
          friend_port = 0
          friend_msg_count = 0
          break

  # user is not a friend and is requesting communication
  else:
    print "Sending busy."
    conn.sendall('BUSY ' + SELF_ID + '\n')
    logsend('BUSY ' + SELF_ID + '\n')
  conn.close()

# start a person handler for each thread
class connHandler(threading.Thread):
  def __init__(self, conn, addr):
    self.conn = conn
    self.addr = addr
    threading.Thread.__init__(self)

  def run(self):
    personHandler(self.conn, self.addr)

# start a listening tcp server
def startServer():
  global SELF_PORT
  global SELF_IP
  global logfile
  server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  server.bind((SELF_IP, SELF_PORT))
  server.listen(5)
  
  while True:
    conn, addr = server.accept()
    connHandler(conn, addr).start()


SERVER_IP = "127.0.0.1"
SERVER_PORT = 5000
SERVER_ADDR = (SERVER_IP, SERVER_PORT)

# create udp connection to the server
udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 

SELF_ID = ''
SELF_IP = ''
SELF_PORT = 0

registered = False

friend = ''
friend_ip = ''
friend_port = 0
friend_msg_count = 0

initiator = ''

def main():
  # Get your app key and secret from the Dropbox developer website
  app_key = 'xuf5vkua1xi09b4'
  app_secret = '6ctdznqpg1lrqth'

  flow = dropbox.client.DropboxOAuth2FlowNoRedirect(app_key, app_secret)
  
  global registered
  global SELF_ID
  global SELF_PORT
  global SELF_IP
  global SERVER_IP
  global SERVER_PORT
  global SERVER_ADDR
  global udp_sock
  global BUFFER
  global friend
  global friend_ip
  global friend_port
  global friend_msg_count
  global initiator
  global logfile
  global logfilename

  if len(sys.argv) is 4:
    SELF_ID = sys.argv[1]
    SERVER_IP = sys.argv[2]
    SERVER_PORT = sys.argv[3]
  if len(sys.argv) is 5:
    SELF_ID = sys.argv[1]
    SERVER_IP = sys.argv[2]
    SERVER_PORT = sys.argv[3]
    logfilename = sys.argv[4]
  else:
    exit("Wrong arguments provided.")

  # open logfile
  logfile = open(logfilename, 'w')

  while True:
    user_input = raw_input('client-' + SELF_ID + '> ').rstrip('\n').split(' ')

    # BEFORE REGISTRATION
    if not registered:
      if user_input[0] == 'REGISTER':
        SELF_IP = user_input[1]
        SELF_PORT = int(user_input[2])

        udp_sock.bind((SELF_IP, SELF_PORT))
        udp_sock.sendto('REGISTER ' + SELF_ID + ' ' + SELF_IP + ' ' + str(SELF_PORT) + '\n', SERVER_ADDR);
    
        logsend('REGISTER ' + SELF_ID + ' ' + SELF_IP + ' ' + str(SELF_PORT) + '\n')

        start = time.time()
        now = time.time()
        has_responded = False
        tries = 0

        # try twice and timeout
        while tries < 2 and not has_responded:
          while not has_responded and now-start<10:
            data = udp_sock.recv(BUFFER)
            data_values = data.rstrip('\n').split(' ')

            if data_values[0] == 'ACK':
              logrecv(data)
              has_responded = True
            
            now = time.time()
          start = time.time()
          tries += 1

        registered = True
        print 'successfully registered'

        # begin handling connections
        try:
          thread.start_new_thread(startServer, ())
        except:
          print "Error: can't create a thread."
      else:
        print "Error: invalid command."

    # AFTER REGISTRATION
    elif user_input[0] == 'QUERY':
      udp_sock.sendto('QUERY ' + user_input[1] + '\n', SERVER_ADDR);
      logsend('QUERY ' + user_input[1] + '\n')
      response = udp_sock.recv(BUFFER)
      logrecv(response)
      print response
    elif user_input[0] == 'DOWN':
      udp_sock.sendto('DOWN ' + user_input[1] + ' ' + user_input[2] + ' ' + user_input[3] + '\n', SERVER_ADDR);
      logsend('DOWN ' + user_input[1] + ' ' + user_input[2] + ' ' + user_input[3] + '\n')
    elif user_input[0] == 'QUIT':
      udp_sock.sendto('QUIT ' + SELF_ID + ' ' + SELF_IP + ' ' + str(SELF_PORT) + '\n', SERVER_ADDR);
      logsend('QUIT ' + SELF_ID + ' ' + SELF_IP + ' ' + str(SELF_PORT) + '\n')
      sys.exit()

    # SPAWN FRIEND CONNECTION THREAD
    elif friend == '' and user_input[0] == 'FRIEND':
      udp_sock.sendto('QUERY ' + user_input[1] + '\n', SERVER_ADDR);
      logsend('QUERY ' + user_input[1] + '\n')
      response = udp_sock.recv(BUFFER).rstrip('\n').split(' ')
      logrecv(response)

      if int(response[3]) == 0:
        print "User does not exist."
      else:
        try:
          # Connect to server and send data
          initiator = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
          initiator.connect((response[2], int(response[3])))

          s = initiator

          s.sendall('FRIEND ' + SELF_ID + "\n")
          logsend('FRIEND ' + SELF_ID + "\n")

          received = s.recv(1024).rstrip('\n').split(' ')
          logrecv(received)
          if received[0] == 'CONFIRM':
            friend = received[1]
            friend_ip = response[2]
            friend_port = int(response[3])
            print "CONFIRM received."
          elif received[0] == 'BUSY':
            print 'That person is busy.'
          else:
            print str(response)
            udp_sock.sendto('DOWN ' + response[1] + ' ' + response[2] + ' ' + response[3] + '\n', SERVER_ADDR);
            logsend('DOWN ' + response[1] + ' ' + response[2] + ' ' + response[3] + '\n')
            print 'Cannot reach user. Sending a DOWN.'

          # spawn threads to handle friendship
          thread.start_new_thread(friendLocalHandler, (s,))
          thread.start_new_thread(friendLocalInputHandler, (s,))
        finally:
          pass
      
    # COMMUNICATE WITH FRIEND CONNECTION THREAD
    elif friend != '':
      queue.put(user_input)
    else:
      print "Error: invalid command."
  logfile.close()
    


if __name__ == "__main__":
    main()

