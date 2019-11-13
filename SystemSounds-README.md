## What are System Sounds
System Sounds are mp3 files which needs to be copied on the TF Card into the folder mp3 and advert. ([See details](MicroCard-README.md
)) Those files are used to navigate through menus and to give users an audio response on their actions.

## How to generate System sounds
There are shell scripts in the folder [/systemSounds](/systemSounds) which can generate text to mp3-files. Inside the [dockerfile](/docker/Dockerfile) you can see what kind of dependencies you need to generate the System Sounds. 
I started using the goolge speech service because it is very convenient to consume it.
