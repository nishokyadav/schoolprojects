CPE 400 Programming Assignment Phase 3
Nishok Yadav
Jared Rhizor
======================================================================================================================
Files:
CloudHandler.py:
Handles the file uploads, downloads, and updates using the cloud services.

P2PNetwork.py:
Handles the P2P aspects including chat, friend requests, profile requests, etc.

socialnetwork.py:
Acts as the main driver, provides the command line arguments to the P2PNetwork class to operate.
======================================================================================================================
How to start our program:
1) 	Navigate to the directory you have our .py files in
2) 	Run the command:
	python socialnetwork.py <user_id> <cloud_service> <directory_of_xml_files>

	Notes:
	<cloud_service> should be either 'drive' or 'dropbox'
	<directory_of_xml_files> should be the path to the directory containing your xml files
======================================================================================================================
We created Google Drive accounts for this:
user jrhizor is rhizoryadav@gmail.com (password: vadayrozihr)
user nyadav is yadavrhizor@gmail.com (password: rozihrvaday)

If you just run the program with 
./socialnetwork.py jrhizor drive jrhizor_files
./socialnetwork.py nyadav drive nyadav_files

it should not require OAuth; it loads from the credentials files.

======================================================================================================================

When a user types QUIT and exits the program gracefully, the friends list is saved to a serialized Python file. Once that user reconnects, he or she can reinitiate their friendship with other users. 

======================================================================================================================

Operating the program once it's started:
Command: FRIEND <user_email_address>
Description: Sending this command sends a friend request to the specified user's email address.

Command: FRIEND_URL <download_link_from_email>
Description: Sending this command with the link you received in email will establish a TCP connection

Command: MSG <user_id> <message>
Description: For the UI, we have MSG as the CHAT command. Using this command will send a message to the specified user. The underlying CHAT command is still sent with the format CHAT <counter> <message>. 

Command: TERMINATE <user_id>
Description: Ends chat with the specified user by closing the TCP connection.

Command: QUIT
Description: Exits the entire socialnetwork program.

Command: PROFILE <user_id>
Description: Displays a link to the user's publicProfile.xml file and each item in the content.xml.

Command: UPDATE <version> <size> <msg>
Description: Sends an updated part of the profile. Since we do not guarantee that the profile is downloaded in advance, we process the segment individually. Additionally, since many of the links are web links, when the file is requested, we assume that the last string after the last / is in the user's directory. That file is sent and displayed in plaintext on the terminal. 
Example:
UPDATE 10 1024 <version id="12"><type>image</type><tag>profile,Mehmet Gunes</tag><time>10/23/2013 1:10pm</time><item>http://www.cse.unr.edu/~mgunes/images/content.xml</item><info>Me</info></version>
======================================================================================================================
Nishok Yadav’s Contribution:
I got the Google Drive API installed and set up the uploading of the files to Google Drive and downloading of the files from the Drive from the emailed link. We made a generic Cloud Handler class and created two subclasses (one for Drive and one for Dropbox) that inherit from the Cloud Handler. This allowed us to separately implement the different cloud services, but easily incorporate them into one program. Other than separately implementing the cloud services, we worked together with pair programming on the P2PNetwork class which took the majority of the time for implementing this assignment.
