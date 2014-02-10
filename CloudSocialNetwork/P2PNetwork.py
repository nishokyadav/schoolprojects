import socket
import sys
import thread
import time
import pickle
import urllib2
import threading
import smtplib
import Queue
import os.path
import xml.etree.ElementTree as ET
import logging

from email.MIMEText import MIMEText
from CloudHandler import *

class P2PNetwork():
  def __init__(self, cloud_provider, directory, self_id):
    self.id = self_id
    self.initialized = False
    self.friends_list = {}
    self.directory = directory

    if os.path.exists(directory + "/friends_list.pickle"):
      # load saved friends database if it exists

      self.friends_list = pickle.load( open( directory + "/friends_list.pickle", "rb" ) )

    # select cloud provider 
    if cloud_provider == 'drive':
      self.cloud = DriveHandler(self_id, directory)
    elif cloud_provider == 'dropbox':
      self.cloud = DropboxHandler(self_id, directory)

  # send friend request email over SMTP
  # for simplicity, uses the same GMAIL SMTP account
  def send_friend_req(self, email):
    msg = MIMEText(self.cloud.get_download_url("location.xml"))
    msg['Subject'] = "P2PNetwork - %s has sent you a friend request." % self.id
    msg['From'] = "yadavrhizor@gmail.com"
    msg['To'] = email

    server = smtplib.SMTP('smtp.gmail.com',587)
    server.ehlo()
    server.starttls()
    server.ehlo()
    server.login('yadavrhizor@gmail.com','rozihrvaday')
    server.sendmail(msg['From'], msg['To'], msg.as_string())
    server.close()

    logging.info("Sending email to " + email)

  # handle all connections coming in to a TCP connection
  def recv_handler(self, p2p_net, conn, user):
    # until they stop being friends
    while user in p2p_net.friends_list:
      received = conn.recv(100024).rstrip('\n').split(' ')

      # checked if the whole message is received
      if received:
        logging.info(' '.join(received))
        # handle chat, send delivered
        if received[0] == "CHAT":
          print user + ': ' + ' '.join(received[3:])
          p2p_net.friends_list[user][4] = p2p_net.friends_list[user][4] + 1
          conn.sendall('DELIVERED ' + received[1] + '\n')
        # handle terminate
        elif received[0] == "TERMINATE":
          conn.close()
          print "Ended friendship with " + user
          del p2p_net.friends_list[user]
          break
        # don't do anything when you get delivered
        elif received[0] == "DELIVERED":
          logging.info('DELIVERED')
        elif received[0] == "UPDATE":
          logging.info('UPDATE received')
          # parse xml representation
          root = ET.fromstring('<?xml version="1.0" encoding="UTF-8"?><content>'+' '.join(received[3:])+'</content>')
          print "Received update: "
          print ' '.join(received[3:])
          item_location = ''

          for version in root.findall('version'):
            item_location = version.find('item').text

          filename = (item_location.split('/'))[-1]
          print "Filename requested:", filename

          conn.sendall('GET ' + filename + '\n')

        elif received[0] == "FILE":
          # output received file
          print "Received file:", received[1], received[2]
          print ' '.join(received[3:])  
          logging.info('FILE received')

        elif received[0] == 'GET':
          f = open(self.directory + '/' + received[1], 'r') # very dangerous
          file_contents = f.read()
          conn.sendall('FILE ' + received[1] + ' ' + str(len(file_contents)) + ' ' + file_contents)
          logging.info('sending FILE')
          f.close()
          logging.info('GET received')

  # handle everything for a connection requested externally
  def respond(self, p2p_net, conn, addr):
    # handle everything for confirmed connection
    received = conn.recv(1024).rstrip('\n').split(' ')
    logging.info(' '.join(received))
    # handle FRIEND request, send confirm
    if received[0] == 'FRIEND':  
      conn.sendall('CONFIRM ' + p2p_net.id + '\n')
      logging.info('sending CONFIRM ' + p2p_net.id + '\n')
      [user, ip, port] = p2p_net.parse_location_xml(received[2])
      p2p_net.friends_list[user] = [ip, port, received[2], Queue.Queue(), 0]     
      print "You are now friends with " + user

      # start a thread to handle incoming requests for this friend
      thread.start_new_thread(self.recv_handler, (p2p_net, conn, user))

      # while they are still friends
      while user in p2p_net.friends_list:
        # if commands are not queued to be sent
        if p2p_net.friends_list[user][3].empty():
          pass
        # if commands are queued to be sent
        else:
          cmd = p2p_net.friends_list[user][3].get()
          # handle sending chat message
          if cmd[0] == 'MSG':
            p2p_net.friends_list[user][4] = p2p_net.friends_list[user][4] + 1
            conn.sendall('CHAT ' + str(p2p_net.friends_list[user][4]) + ' ' + ' '.join(cmd[1:]) + '\n')
            logging.info('sending CHAT ' + str(p2p_net.friends_list[user][4]) + ' ' + ' '.join(cmd[1:]) + '\n')
          # handle terminating friendship
          elif cmd[0] == 'TERMINATE':
            conn.sendall('TERMINATE' + '\n')
            logging.info('sending TERMINATE to' + user)
            conn.close()
            print "Ended friendship with " + user
            del p2p_net.friends_list[user]
            break
          elif cmd[0] == 'UPDATE':
            conn.sendall(' '.join(cmd) + '\n')
            logging.info(' '.join(cmd) + '\n')
            

  # handle initiating contact with another user
  def send_tcp_friend_req(self, p2p_net, url):
    [user, ip, port] = p2p_net.parse_location_xml(url)

    # if the user is online 
    if port != 0:
      # create TCP connection
      server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      server.connect((ip, port))

      # send them a friend request
      server.sendall('FRIEND ' + p2p_net.id + ' ' + p2p_net.cloud.get_download_url('location.xml') + '\n')
      logging.info('sending FRIEND ' + p2p_net.id + ' ' + p2p_net.cloud.get_download_url('location.xml') + '\n')

      # wait to receive a response in this thread
      received = server.recv(1024).rstrip('\n').split(' ')

      logging.info(' '.join(received))

      # if the other user confirms 
      if received[0] == 'CONFIRM':   
        # start a thread to handle all responses from the other user
        thread.start_new_thread(self.recv_handler, (p2p_net, server, user))
        
        p2p_net.friends_list[user] = [ip, port, url, Queue.Queue(), 0]     
        print "You are now friends with " + user

        # while they are still friends
        while user in p2p_net.friends_list:
          if p2p_net.friends_list[user][3].empty():
            pass
          # if there are queued commands
          else:
            cmd = p2p_net.friends_list[user][3].get()
            # handle sending a message
            if cmd[0] == 'MSG':
              p2p_net.friends_list[user][4] = p2p_net.friends_list[user][4] + 1
              server.sendall('CHAT ' + str(p2p_net.friends_list[user][4]) + ' ' + ' '.join(cmd[1:]) + '\n')
              logging.info('sending CHAT ' + str(p2p_net.friends_list[user][4]) + ' ' + ' '.join(cmd[1:]) + '\n')
            # handle terminating friendship
            elif cmd[0] == 'TERMINATE':
              server.sendall('TERMINATE' + '\n')
              logging.info('sending TERMINATE to ' + user)
              server.close()
              print "Ended friendship with " + user
              del p2p_net.friends_list[user]
              break
            elif cmd[0] == 'GET':
              f = open(cmd[1], 'r') # very dangerous
              file_contents = f.read()
              server.sendall('FILE ' + cmd[1] + ' ' + len(file_contents) + ' ' + file_contents)
              logging.info('sending FILE')
              f.close()
            elif cmd[0] == 'UPDATE':
              server.sendall(' '.join(cmd) + '\n')
              logging.info(' '.join(cmd) + '\n')
      else:
        server.close()
        print "ERROR: Confirmation malformed."

    else:
      print "ERROR: That user is not currently online."

  # get the user, ip, and port numbmer for a specified online location.xml 
  def parse_location_xml(self, url):
    response = urllib2.urlopen(url)
    xml = response.read()

    root = ET.fromstring(xml)

    user = ''
    ip = ''
    port = 0

    for address in root.findall('address'):
      user = address.find('ID').text
      ip = address.find('IP').text
      port = address.find('port').text

    return [user, ip, int(port)]

  # get the public profile URL as well as all of the link items in content.xml
  def parse_content_xml(self, location_url):
    response = urllib2.urlopen(location_url)
    xml = response.read()
    root = ET.fromstring(xml)

    content_location = ''
    public_location = ''

    for link in root.findall('links'):
      public_location = link.find('public').text
      content_location = link.find('content').text

    links = [public_location]

    response = urllib2.urlopen(content_location)
    xml = response.read()
    root = ET.fromstring(xml)    

    for version in root.findall('version'):
      for item in version.findall('item'):
        links.append(item.text)

    return links

  # listen for any incoming connections
  def startListener(self, p2p_net):
    # connect with temporary socket to determine external IP
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("gmail.com",80))
    p2p_net.ip = s.getsockname()[0]
    s.close()

    # create TCP listening socket
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(("", 0))
    server.listen(5)
  
    # get port bound to listener
    p2p_net.port = server.getsockname()[1]

    # update and upload files to cloud
    p2p_net.cloud.upload()
    p2p_net.cloud.update_location(p2p_net)
    p2p_net.cloud.upload()

    p2p_net.initialized = True

    # always listen for new connections
    while True:
      conn, addr = server.accept()
      # create handler for new incoming connections
      thread.start_new_thread(self.respond, (self, conn, addr))

  # start the program
  def run(self):    
    print "Starting, please wait."

    # start listening for incoming connections
    thread.start_new_thread(self.startListener, (self,))

    # wait for the listener to initialize
    while not self.initialized:
      pass

    # for each friend
    for friend in self.friends_list:
      # create a tcp connection with them
      self.friends_list[friend][3]=Queue.Queue()
      thread.start_new_thread(self.send_tcp_friend_req, (self,self.friends_list[friend][2]))

    # handle user inputs
    while True:
      user_input = raw_input('client-' + self.id + '> ').rstrip('\n').split(' ')

      # FRIEND <email>
      if user_input[0] == 'FRIEND' and len(user_input) == 2:
        self.send_friend_req(user_input[1])

      # FRIEND_URL <other_user_location_url>
      elif user_input[0] == 'FRIEND_URL' and len(user_input) == 2:
        thread.start_new_thread(self.send_tcp_friend_req, (self,user_input[1]))

      # LIST
      elif user_input[0] == 'LIST':
        for friend in self.friends_list:
          print friend

      # MSG <user> <your message>
      elif user_input[0] == 'MSG' and len(user_input) > 2:
        if user_input[1] in self.friends_list:
          self.friends_list[user_input[1]][3].put(user_input)
        else:
          print "ERROR: Can't send message to people that aren't your friends."
    
      # TERMINATE <user>
      elif user_input[0] == 'TERMINATE' and len(user_input) == 2:
        if user_input[1] in self.friends_list:
          self.friends_list[user_input[1]][3].put(user_input)
        else:
          print "ERROR: Can't terminate someone not on your friends list."

      # PROFILE <user>
      elif user_input[0] == 'PROFILE' and len(user_input) == 2:
        if user_input[1] in self.friends_list:
          print self.parse_content_xml(self.friends_list[user_input[1]][2])
        else:
          print "ERROR: Can't view the profile of someone not on your friends list."

      # UPDATE <version_number> <length> <xml_content>
      elif user_input[0] == 'UPDATE' and len(user_input) > 4:
        for friend in self.friends_list:
          self.friends_list[friend][3].put(user_input)

      # QUIT
      elif user_input[0] == 'QUIT':
        self.cloud.sign_off()


        # save friends list
        for friend in self.friends_list:
          self.friends_list[friend] = [self.friends_list[friend][0]] + [self.friends_list[friend][1]]+ [self.friends_list[friend][2]] + [0] + [self.friends_list[friend][4]]

        print self.friends_list
        pickle.dump( self.friends_list, open( self.directory + "/friends_list.pickle", "wb" ) )

        exit("Bye.")

      else:
        print "ERROR: Command not recognized."
