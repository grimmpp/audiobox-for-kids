# AudioBox for Kids (Arduino and DFMiniPlay Project)
This project is about an audiobox which allows small kids, who cannot read, don't understand signs, or don't know which button to push, an easy way to choose sound effects, audio books, or music tracks. This can be done by putting small items on the top of the audiobox. Those items are related to the e.g. audio book they want to play and can be e.g. a small figure or a labelled cards or tags. 
<br />
<table border=0><tr>
  <td><img src="/pics/AudioboxForKids.png" width=300/></td>
  <td><img src="/pics/Board_Description_small.jpg" width=300/></td>
  <td><img src="/pics/20190930_191049_small.jpg" width=300/></td>
</tr></table>

# What can it do?
The audiobox plays a mp3 file which is storage on a TF card after a RFID/NFC tag was placed on top of the box. 

## Preparation
### Copy mp3 files on TF Card
First of all you need to copy the mp3 files which you want to listen to onto the TF card. Therefore have a look how the structure must look like. (TODO: add link) <br />
### Configure NCF Tag
Before a tag can be used you need to tell the audiobox what should be played. After placing the tag on top of the audiobox the first time the audiobox will ask you and guide you through a menu in order to configure the tag. The following relevant infromation will be stored on the tag: folder number, mode, special field. You need to select a folder in which the mp3 is placed and the Arduino will store the folder number on the tag. After that you need to choose a play mode which is Audio Book, Album, Party, Single Track, Audio Book with Bookmark, or Admin. If you choose Single Track or Admin mode the menu will ask for another track or informaiton that will be stored into the special field. (Bookmarks will be automatically stored direktly on the Arduino's EEPROM so that you can continue listening furhter tracks of an audiobox. Bookmarks don't require space on the tag.)

## Play Modes
* **Audio Book**
* **Album**
* **Party**
* **Single Track** <br />
  By placing a NCF Tag on the audiobox it will play only one single track which is configured on the NFC Tag and stops afterwards. <br />
  Button next song and previous song is disabled in this mode.  <br />
  Button play/pause pauses, continuous, or restarts the track after it was finished.
* **Audio Book with Bookmark**
  After placing a tag on top of the box it starts playing an episode. After finishing an episode it stores the number of the next episode on the EEPROM (Arduino's internal persistent memory) so that it can proceed from there next time when the same tag is placed. If not interrupted by the pause button or another tag it will continue playing the next episode. 

# Hardware
* **[Part List](/part-list.md)** <br />
<img src="/electronics/Schematic.png" height=200 />

# Arduino Software
## PlatformIO commands:
* Build code: `pio run`
* Build and upload code: `pio run -v -t upload`
* Open serial monitor: `pio device monitor -b 115200`

## Build with Docker:
Todo: ...

# Thanks to Thorsten Voß
Thorsten Voß had the brilliant idea to build such a audiobox for little kids. He realized his idea also with almost the same parts. His tutorial and big community is really great and inspired me a lot.
* [Website TonUINO](https://www.voss.earth/tonuino/)
* [Community TonUINO](https://discourse.voss.earth/)
* [GitHub Repository TonUINO](https://github.com/xfjx/TonUINO)

In the community a lot of hard- and software issues are discussed and solved. There are also a [big variety of ideas who to build a case](https://discourse.voss.earth/t/tonuino-gehaeuse-galerie/786). Thorsten also offers a [board](https://discourse.voss.earth/t/neue-platine-version-1-51/63) on which you can put the Arduino Nano, DfMiniPlayer, and to which you can connect the RFID Reader. (Wake up logic for Power Bank is not included.)

## Links
* [DFRobot: DfPlayer Mini](https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299)
* [Lib: DFPlayer Mini Mp3 by Makuna](https://github.com/Makuna/DFMiniMp3/blob/master/src/DFMiniMp3.h)
* [Lib: JC_Button](https://github.com/JChristensen/JC_Button)
