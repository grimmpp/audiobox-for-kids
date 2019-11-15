## What are System Sounds
System Sounds are mp3 files which needs to be copied on the TF Card into the folder mp3 and advert. ([See details](MicroCard-README.md
)) Those files are used to navigate through menus and to give users an audio response on their actions.

## How to generate System sounds
There are bash scripts in the folder [/systemSounds](/systemSounds) which can generate text to mp3-files. Inside the [dockerfile](/docker/Dockerfile) you can see what kind of dependencies you need to generate the System Sounds. The System Sounds will be placed into the same directory [/systemSounds](/systemSounds). <br />
I started using the goolge speech service because it is very convenient to consume. You only need internet and some python libs for it.

Bevor executing the commands from the table below ensure you changed into [/systemSounds](/systemSounds). 

| Language | Service | Command | 
| ---|---|---|
|German|Google|**[./createSystemSounds_google_de.sh](/systemSounds/createSystemSounds_google_de.sh)**|
|English|Google|**[./createSystemSounds_google_en.sh](/systemSounds/createSystemSounds_google_en.sh)**|

## Say from MacOS
I really like the text to speech [say](https://ss64.com/osx/say.html) tool from MacOS. Unfortunately I don't own a Mac and so far I don't know a way to use this tool without having MacOS running. Luckily Thorsten Voß provides a [script](https://github.com/xfjx/TonUINO/blob/master/create-soundfiles.sh) and [pregenerated files](https://github.com/xfjx/TonUINO/tree/master/SD-Karte) in his git repository: [TonUINO](https://github.com/xfjx/TonUINO).
