FROM ubuntu:latest
# avr-gcc is using glibc which is not available in alpine => ubuntu

# install ubuntu packages for platformio
RUN apt-get update  && \
	# install python 2
	apt-get install -y python-pip python && \
	# install git
	apt-get install -y git && \
	# delivers lsusb for checking / troubleshooting usb ports
	apt-get install -y usbutils

# install platformio
RUN pip install -U platformio

# clone repo
RUN cd /home && \
	git clone https://github.com/grimmpp/audiobox-for-kids.git


### BUILD CODE for Arduino Nano
# 
# run platformio: download all dependencies and build projects
RUN cd /home/audiobox-for-kids && pio run -v


### Install google text to speech (mp3)
# 
# install packages for google_speech
RUN apt-get install -y python3 python3-pip sox libsox-fmt-mp3
# 
# install google's text to speech
RUN pip3 install google_speech


### Generate German System Sounds
# 
# set language to German | needed for special characters
RUN apt-get install locales	
RUN locale-gen de_DE 
RUN locale-gen de_DE.UTF-8
ENV LANG de_DE.UTF-8
ENV LANGUAGE de_DE:de
ENV LC_ALL de_DE.UTF-8

# Generate German mp3-files
RUN cd /home/audiobox-for-kids/systemSounds && \
	chmod 777 ./createSystemSounds_google_de.sh && \
	./createSystemSounds_google_de.sh

# Generate English mp3-files
RUN cd /home/audiobox-for-kids/systemSounds && \
	chmod 777 ./createSystemSounds_google_en.sh && \
	./createSystemSounds_google_en.sh


### Change into this folder:
## cd docker
##
### Run to build docker image:
## docker build -t audiobox .
##
### Start container containing repo + built arduino SW + generated mp3 files:
## docker run -it audiobox


### if you want to upload the code to arduino run 
### if you use docker toolbox enable usb first in virtualbox in your VM settings. PIO will detect automatically the right usb port.
##
## docker run -it --privileged audiobox 
## Inside the container: cd /home/audiobox && pio run -v -t upload
##
### OR (the following starts the container, executes the pio upload command, and stops the container agian)
##
## docker run --privileged -w /home/audiobox audiobox pio run -v -t upload


### alternatively use existing git repo from host. 
### The following starts the container and mounts your working dir from the host.:
##
## docker run -it -v /c/Users/User/Documents/AudioBoxForKids/AudioBoxForKids:/home/audiobox -w /home/audiobox audiobox
