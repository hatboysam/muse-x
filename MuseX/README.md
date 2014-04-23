# Setup
Run the following commands to allow Java to talk to Arduino:
	
	sudo mkdir /var/lock
	sudo chmod 777 /var/lock

Then, from the MuseX directory, run the following command to install the needed library:

	sudo cp lib/librxtxSerial.jnilib /System/Library/Java/Extensions/
