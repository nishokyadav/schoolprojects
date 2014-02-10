#!/usr/bin/env python

from CloudHandler import *
from P2PNetwork import *
import logging

def main():
  if len(sys.argv) is 4:
    self_id = sys.argv[1]
    cloud_provider = sys.argv[2]
    directory = sys.argv[3]
    if cloud_provider != "drive" and cloud_provider != "dropbox":
      exit("ERROR: the cloud provider must be drive or dropbox.")
  else:
    exit("ERROR: Incorrect arguments. Format is: <self_id> <cloud_provider> <directory>")

  # setup logging
  logging.basicConfig(filename=directory+'/activity.log', filemode='w', level=logging.INFO)

  logging.info('Starting program')

  prog = P2PNetwork(cloud_provider, directory, self_id)
  prog.run()

  logging.info('Ending program')

if __name__ == "__main__":
    main()