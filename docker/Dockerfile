FROM ubuntu:latest

# install ubuntu packages
RUN apt-get update  && \
	# install python 2
	apt-get install -y python-pip python	&& \
	# install git
	apt-get install -y git 

# install platformio
RUN pip install -U platformio

# clone repo
RUN cd /home && git clone https://github.com/grimmpp/audiobox-for-kids.git

# run platformio: download all dependencies and build projects
RUN cd /home/audiobox-for-kids && pio run -v
