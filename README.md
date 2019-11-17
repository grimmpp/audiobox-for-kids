# AudioBox for Kids (Arduino and DFMiniPlayer Project)
This project is about an audiobox which allows small kids, who cannot read, don't understand signs, or don't know which button to push, an easy way to choose sound effects, audio books, or music tracks. Playing tracks can be done by putting small items on the top of the audiobox. Those items are related to the e.g. audio book kids want to play and can be e.g. small figures or labelled cards or other NFC tags. 
<br />
<table border=0><tr>
  <td><img src="/pics/AudioboxForKids.png" width=300/></td>
  <td><img src="/pics/Board_Description_small.jpg" width=300/></td>
  <td><img src="/pics/20190930_191049_small.jpg" width=300/></td>
</tr></table>

## Content
* **[What can it do?](#what-can-it-do)**
  * [Play Modes](#play-modes)
  * [Features](#features)
* **[Administration](#administration)**
  * [Copy mp3 files on TF Card (Micro SD Card)](#copy-mp3-files-on-tf-card-micro-sd-card)
  * [Configure NFC Tag](#configure-nfc-tag)
* **[Hardware](#hardware)**
  * [Part List](#part-list)
  * [Schema](#schema)
  * [Case](#case)
* **[Arduino Software](#arduino-software)**
  * [Why did I choose VS Code and PlatformIO?](#why-did-i-choose-vs-code-and-platformio)
  * [PlatformIO commands](#platformio-commands)
  * [Build with Docker](#build-with-docker)
* **[Sound Tracks](#sound-tracks)**
  * [Animal Sounds](#animal-sounds)
  * [Sound Effects](#sound-effects)
  * [Free Audio Books](#free-audio-books)
  * [Cutting Tool Recommendation](#cutting-tool-recommendation)
  * [Structure of TF Card](#structure-of-tf-card)
  * [How to generate System Sounds for menu?](#how-to-generate-system-sounds-for-menu)
* **[Thanks to Thorsten Voß](#thanks-to-thorsten-voß)**
* **[Links](#links)**

# What can it do?
The audiobox plays a mp3 file which is storage on a TF card (small SD Card) after a RFID/NFC Tag was placed on top of the box. This is the basic function of the audiobox. You can also use the button on top of the audiobox in order to change the volume, switch to the next or previous track, or to pause a audio track.

## Play Modes
* **Audio Book** <br />
After placing a tag on top of the audiobox it starts playing a random track out of the defined folder which is stored on the RFID Tag. After finishing the track the audiobox stops playing other tracks.
* **Album** <br />
Plays all track from the defined folder in the order in which the tracks are stored on the TF Card.
* **Party** <br />
Plays tracks randomly from the defined folder. It does not stop until another RFID Tag is placed or the pause button was pushed.
* **Single Track** <br />
  By placing a NCF Tag on the audiobox it will play only one single track which is configured on the NFC Tag and stops afterwards. <br />
  Button next song and previous song is disabled in this mode.  <br />
  Button play/pause pauses, continuous, or restarts the track after it was finished.
* **Audio Book with Bookmark** <br />
  After placing a tag on top of the box it starts playing an episode. After finishing an episode it stores the number of the next episode on the EEPROM (Arduino's internal persistent memory) so that it can proceed from there next time when the same tag is placed. If not interrupted by the pause button or another tag it will continue playing the next episode. 

## Features
* Plays tracks, albums, songs, audio books, ... by placing NFC Tags on the box.
* Bookmarks for audio books
* Random track selection
* Menu to configure and map the NFC Tags
* Timeout for menus (default 60s)
* Start sound which plays after arduino is initialized
* Button combination to restart Arduino
* Keep alive logic for power banks (See [PowerBankKeepAliveManager.h](/lib/PowerBankKeepAliveManager/PowerBankKeepAliveManager.h))
* Delay for button for long press mode (See [ButtonManager.h](/lib/ButtonManager/ButtonManager.h))
* Beep which will be played if a button is pushed (planned)



# Administration

## Copy mp3 files on TF Card (Micro SD Card)
Before you can start you need to copy the mp3 files which you want to listen to onto the TF card. Therefore have a look how the structure of the TF Card must look like. In addition to the sound file you want to listen to you need to copy some file for the menus. For deteails have a look [here](/MicroCard-README.md).

## Configure NFC Tag
Before a tag can be used you need to tell the audiobox what should be played. After placing the tag on top of the audiobox the first time the audiobox will ask you and guide you through a menu in order to configure the tag. The following relevant infromation will be stored on the tag: folder number, mode, special field. You need to select a folder in which the mp3 is placed and the Arduino will store the folder number on the tag. After that you need to choose a play mode which is Audio Book, Album, Party, Single Track, Audio Book with Bookmark, or Admin. If you choose Single Track or Admin mode the menu will ask for another track or informaiton that will be stored into the special field. (Bookmarks will be automatically stored direktly on the Arduino's EEPROM so that you can continue listening furhter tracks of an audiobox. Bookmarks don't require space on the tag.)


# Hardware

## [Part List](/part-list.md)

## Schema
<img src="/electronics/Schematic.png" height=200 />

## Case
The case is a simple wooden cube (20cm x 20xm x 20cm). It has three buttons at the top, a speaker at the front, 4 feet at the bottom, and a power jack and a power on/off switch at the back. Inside there is a power bank, the board containing Arduino Nano, DfMiniPlayer, ... and some cable to connect everything.
<table border=0><tr>
  <td><img src="/pics/AudioboxForKids.png" width=300 /></td>
  <td><img src="/case/OpenscadCase.png" width=300 /></td>
  <td><img src="/pics/PHOTO_20191102_210156_small.jpg" width=300 /></td>
</tr></table>

You can find further pictures from the box in folder [/pics](/pics) and the drawing in folder [/case](/case).

## NFC Tags
In the **[Part List](/part-list.md)** you can find the used NFC Tags. I've created as well small figures from an old wooden puzzle and disassembled NFC Tags which I just fixed with ordenary tape. 
<table border=0><tr>
  <td><img src="/pics/PHOTO_20191026_105735_small.jpg" width=300/></td>
  <td><img src="/pics/PHOTO_20191026_172736_small.jpg" width=300/></td>
  <td><img src="/pics/PHOTO_20191026_172803_small.jpg" width=300/></td>
</tr></table>

### TODO: How to imprint RFID Cards

# Arduino Software
I used [PlatformIO](https://platformio.org/) and [VS Code](https://code.visualstudio.com/) in order to develop the software for this project which runs on an Arduino Nano.
## Why did I choose VS Code and PlatformIO?
* [VS Code](https://code.visualstudio.com/) is a proper IDE
  * Shows instantly compiler information
  * Has autocompletion
  * Shows documentation of libraries and functions
  * Shows e.g. what a library supports by using autocompletion
  * Terminal is attached below code window part
  * ...
* [PlatformIO](https://platformio.org/) 
  * Can really easily resolve C-dependencies (dependent libraries)
  * Can be used independently from IDE in command line.
  * Code verification in docker container possible. 
    * Independent of local environment and easy reproducable
  * Supports a lot of controllers and ICs.
  * Code can be easily split up.
  * Projects can be organized.
  * Upload and monitoring to/of Arduino Nano easily possible.
  * ...

## PlatformIO commands
* Build code: **`pio run`**
* Build and upload code: **`pio run -v -t upload`**
* Open serial monitor: **`pio device monitor -b 115200`**

## Build with Docker
**`docker build -t audiobox /docker/Dockerfile`** <br />
This command builds a docker image based on ubuntu. It contains [platformio](https://platformio.org/), downloads this git repo and builds the code and audio files for the audiobox. Building everything the first time takes quite a while until all dependencies are downloaded and everything is built. Please, be a bit patient. :-)

### Upload binaries onto Arduino with docker
After creating the docker image, which also builds the project automatically, you can start the docker container and upload the binaries onto an Arduino Nano by using the following command: <br />
**`docker run -w /home/audiobox-for-kids --privileged audiobox pio run -v -t upload`** <br />
PlatformIO will automatically search for the usb port in which your Arduino Nano is plugged in. <Br />
*--privileged* is used in this case because we need access to the usb ports. Alternatively you can also enable the usb ports by specifying an argument which maps the host usb ports to container usb ports.

### Docker commands
* Build docker image: **`docker build -t audiobox /docker/Dockerfile`**
* Run docker image: **`docker docker run -it audiobox`**
* Upload to Arduino Nano: **`docker run -w /home/audiobox-for-kids --privileged audiobox pio run -v -t upload`**

Inside the docker image you can e.g. run the platformIO commands.



# Sound Tracks
Beside music and audio books which you own, I was interested in sound effects e.g. for the start sound or when a button gets pushed. Below you can find a list of links where I was able to find the one or other nice sound effect which answered my purpose. 

## Animal Sounds
I've created some little wooden figures with RFID Tag on the back. The corresponding sound files I've used you can find on the following listed page:
* [Free Animal Sounds](https://freeanimalsounds.org/de/download-tierstimmen/)

## Sound Effects
* [soundjay.com](https://www.soundjay.com/)
* [partnersinrhyme.com](https://www.partnersinrhyme.com/)
* [freesoundeffects.com](https://www.freesoundeffects.com/)
* [soundbible.com](http://soundbible.com/)

## Free Audio Books
* [gratis-hoerspiele.de](https://www.gratis-hoerspiele.de/)

## Cutting Tool Recommendation
Sometimes sound tracks start with a little silence in the beginning, especially if it is a sound effect or animal sound I want the audiobox to respond directly on the ncf tag. That's why I often remove this little silence in the beginning or end of sound tracks. Often it is also needed to adapt the volume of a sound track. I haven't tried out a lot of tools but I'm quite happy with [mp3DirectCut](https://mpesch3.de/) and that's why I recommend it here.

## [Structure of TF Card](MicroCard-README.md)

## [How to generate System Sounds for menu?](SystemSounds-README.md)



# Thanks to Thorsten Voß
Thorsten Voß had the brilliant idea to build such a audiobox for little kids. He realized his idea also with almost the same parts. His tutorial and big community is really great and inspired me a lot.
* [Website TonUINO](https://www.voss.earth/tonuino/)
* [Community TonUINO](https://discourse.voss.earth/)
* [GitHub Repository TonUINO](https://github.com/xfjx/TonUINO)

In the community a lot of hard- and software issues are discussed and solved. There is also a [big variety of ideas how to build a case](https://discourse.voss.earth/t/tonuino-gehaeuse-galerie/786). Thorsten also offers a [board](https://discourse.voss.earth/t/neue-platine-version-1-51/63) on which you can put the Arduino Nano, DfMiniPlayer, and to which you can connect the RFID Reader. (Wake up logic for Power Bank is not included.)

# Links
* [PlatformIO](https://platformio.org/) 
* [VS Code](https://code.visualstudio.com/)
* [DFRobot: DfPlayer Mini](https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299)
* [DF Mini Player - Datasheet](/FN-M16P+Embedded+MP3+Audio+Module+Datasheet.pdf) ([original link](https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/doc/FN-M16P%2BEmbedded%2BMP3%2BAudio%2BModule%2BDatasheet.pdf))
* [Lib: DFPlayer Mini Mp3 by Makuna](https://github.com/Makuna/DFMiniMp3/blob/master/src/DFMiniMp3.h)
* [Lib: JC_Button](https://github.com/JChristensen/JC_Button)
* [mp3DirectCut](https://mpesch3.de/)
* [Website TonUINO](https://www.voss.earth/tonuino/)
* [Community TonUINO](https://discourse.voss.earth/)
* TonUINO Repos
  * [GitHub Repo original TonUINO from Thorsten Voß](https://github.com/xfjx/TonUINO)
  * [GitHub Repo TonUINO from uluzox](https://github.com/uluzox/TonUINO)
