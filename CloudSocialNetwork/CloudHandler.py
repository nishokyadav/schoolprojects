import dropbox
import os
import httplib2
import abc
import xml.etree.ElementTree as ET
from time import localtime, strftime
import pickle
import logging
from memoized import *

from apiclient.discovery import build
from apiclient import errors
from apiclient.http import MediaFileUpload
from oauth2client.client import OAuth2WebServerFlow

# Google Drive constants
CLIENT_ID = '1040609338656-1v2o7bl1vkoa4t474l1n5g7dkd319046.apps.googleusercontent.com'
CLIENT_SECRET = '_WzywaSraMaQqVXwJcwi7nqb'
OAUTH_SCOPE = 'https://www.googleapis.com/auth/drive'
REDIRECT_URI = 'urn:ietf:wg:oauth:2.0:oob'

# Dropbox constants
app_key = 'xuf5vkua1xi09b4'
app_secret = '6ctdznqpg1lrqth'

# a parent class for handling cloud storage 
class CloudHandler():
  # allows abstract method definitions
  __metaclass__ = abc.ABCMeta

  def __init__(self, directory):
    self.directory = directory

    # removing trailing slash if there is one
    if self.directory[-1] == '/':
      self.directory = self.directory[:-1]

    # define relative paths
    self.location_file = self.directory + '/location.xml'
    self.profile_file = self.directory + '/publicProfile.xml'
    self.content_file = self.directory + '/content.xml'
    
    # fail if a file doesn't exist
    assert os.path.isfile(self.location_file)
    assert os.path.isfile(self.profile_file)
    assert os.path.isfile(self.content_file)
  
  @abc.abstractmethod
  def upload_file(self, server_file, local_file):
    return

  @abc.abstractmethod
  def download_file(self, server_file):
    return

  @abc.abstractmethod
  def get_download_url(self, server_file):
    return

  # edit the local file to change the IP and port to 0's, upload
  def sign_off(self):
    location = ET.parse(self.location_file)
    root = location.getroot()
    
    time_str = strftime("%m/%d/%Y %I:%M%p", localtime()).lower()
    
    for address in root.findall('address'):
      address.find('IP').text = "0.0.0.0"
      address.find('port').text = str(0)
      address.find('time').text = time_str

    location.write(self.location_file)

    self.upload_file('location.xml', self.location_file)

  # upload all three files
  def upload(self):
    self.upload_file('location.xml', self.location_file)
    self.upload_file('publicProfile.xml', self.profile_file)
    self.upload_file('content.xml', self.content_file)
  
  # update the location.xml locally to reflect the current IP, port, time, and ID used to run the program
  def update_location(self, p2p_net):
    location = ET.parse(self.location_file)
    root = location.getroot()
    
    time_str = strftime("%m/%d/%Y %I:%M%p", localtime()).lower()
    
    for address in root.findall('address'):
      address.find('IP').text = p2p_net.ip
      address.find('port').text = str(p2p_net.port)
      address.find('time').text = time_str
      address.find('ID').text = p2p_net.id

    for link in root.findall('links'):
      link.find('public').text = self.get_download_url('publicProfile.xml')
      link.find('content').text = self.get_download_url('content.xml')

    location.write(self.location_file)

# handle cloud connection to Google Drive
# IMPORTANT: remember to make your files public on Google Drive
class DriveHandler(CloudHandler):
  def __init__(self, self_id, directory):
    CloudHandler.__init__(self, directory)

    # automatically use saved credentials for saved accounts (see README)
    if self_id == "jrhizor":
      self.credentials = pickle.load( open( "credentials_drive_jrhizor.txt", "rb") )
    elif self_id == "nyadav":
      self.credentials = pickle.load( open( "credentials_drive_nyadav.txt", "rb") )
    else:
      # authorize the user for Google Drive
      self.flow = OAuth2WebServerFlow(CLIENT_ID, CLIENT_SECRET, OAUTH_SCOPE, REDIRECT_URI)
      self.authorize_url = self.flow.step1_get_authorize_url()
      print 'Go to the following link in your browser: ' + self.authorize_url
      self.code = raw_input('Enter verification code: ').strip()
      self.credentials = self.flow.step2_exchange(self.code)
      # pickle.dump( self.credentials, open( "credentials.txt", "wb" ) )

    http = httplib2.Http()
    http = self.credentials.authorize(http)

    self.drive_service = build('drive', 'v2', http=http)

  # upload a local file to a position on the server
  def upload_file(self, server_file, local_file):
    if len(self.retrieve_all_files(server_file))==0:
      media_body = MediaFileUpload(local_file, mimetype='text/plain', resumable=True)
      body = {
        'title': server_file,
        'description': server_file,
        'mimeType': 'text/plain'
      }
      file = self.drive_service.files().insert(body=body, media_body=media_body).execute()

      logging.info('Uploading ' + local_file + ' to cloud ' + server_file)
    else:
      self.update_file(server_file, local_file)

  # update a file on the server - this function is heavily based on the example drive API code
  def update_file(self, server_file, local_file):
    logging.info('Updating ' + local_file + ' in cloud ' + server_file)
    try:
      file = self.retrieve_all_files(server_file)[0]

      # File's new content.
      media_body = MediaFileUpload(local_file, mimetype=file['mimeType'], resumable=True)

      # Send the request to the API.
      updated_file = self.drive_service.files().update(body=file, fileId=file['id'], media_body=media_body).execute()
      return updated_file
    except errors.HttpError, error:
      print 'An error occurred: %s' % error
      return None

  # get a list of all files matching the server file - this function is heavily based on the example drive API code
  def retrieve_all_files(self, server_file):
    result = []
    page_token = None
    while True:
      try:
        param = {}
        if page_token:
          param['pageToken'] = page_token
        files = self.drive_service.files().list(q="title = '" + server_file + "' and trashed = false").execute()

        result.extend(files['items'])
        page_token = files.get('nextPageToken')
        if not page_token:
          break
      except errors.HttpError, error:
        print 'An error occurred: %s' % error
        break
    return result

  # get a direct link to a file
  @memoized
  def get_download_url(self, server_file):
    file = self.retrieve_all_files(server_file)[0]
    return file.get('webContentLink')

  # get the contents of a file
  def download_file(self, server_file):
    logging.info('Downloading from cloud ' + server_file)
    file = self.retrieve_all_files(server_file)[0]
    download_url = file.get('downloadUrl')

    if download_url:
      resp, content = self.drive_service._http.request(download_url)
      if resp.status == 200:
        return content
      else:
        print 'An error occurred: %s' % resp
        return None
    else:
      return None


# handle cloud connection with DropBox
class DropboxHandler(CloudHandler):
  def __init__(self, self_id, directory):
    CloudHandler.__init__(self, directory)
    # authorize dropbox user
    flow = dropbox.client.DropboxOAuth2FlowNoRedirect(app_key, app_secret)
    authorize_url = flow.start()
    print '1. Go to: ' + authorize_url
    print '2. Click "Allow" (you might have to log in first)'
    print '3. Copy the authorization code.'
    code = raw_input("Enter the authorization code here: ").strip()

    # This will fail if the user enters an invalid authorization code
    access_token, user_id = flow.finish(code)

    self.client = dropbox.client.DropboxClient(access_token)

  # delete and reupload the file
  def update_file(self, server_file, local_file):
    self.client.file_delete('/'+server_file)
    f = open(local_file)
    response = self.client.put_file('/'+server_file, f)
    logging.info('Updating ' + local_file + ' in cloud ' + server_file)

  # upload file if it doesn't exist; otherwise, update it 
  def upload_file(self, server_file, local_file):
    if len(self.client.search('/',server_file)) == 0:
      f = open(local_file)
      response = self.client.put_file('/'+server_file, f)
      logging.info('Uploading ' + local_file + ' to cloud ' + server_file)
    else:
      self.update_file(server_file, local_file)

  # get direct download url for file
  @memoized
  def get_download_url(self, server_file):
    resp = self.client.media('/' + server_file)
    return resp['url']

  # download file contents
  def download_file(self, server_file):
    logging.info('Downloading from cloud ' + server_file)
    f, metadata = client.get_file_and_metadata('/' + server_file)
    return f.read()