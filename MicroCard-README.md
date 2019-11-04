## Structure of TF Card (Micro SD Card)

Below you can find the structure of the TF Card.

### Sotring tracks on TF Card
For storing mp3 and wav tracks you can create folder starting with 01 until 99.
Each folder can contain between 1 and 255 mp3 tracks. The track names must start with numbers. Optionally names can be added after the number.

### System Sounds
There are two special folders *mp3* and *advert*. Those two folder contain the file for the system sounds. The system sounds are prepared in this project.

### Known difficulties and recommendations
* Tracks must be stored in the same order like they are named onto the TF Card. Otherwise the mapping will not work.
* I do not recommend to put file into the main folder.
* Use simple filenames
* Do not store other files on the TF Card because this can confuse the DfMiniPlayer
  * Mac users can use this command in order to get rid of unnecessary files: `$ dot_clean /Volumes/SD-Karte`


```
sd:/
   ├── 01
       ├── 001_*.mp3
       ├── ...
       ├── 255_*.mp3
   ├── ...
   ├── 99
       ├── 001_*.mp3
       ├── ...
       ├── 255_*.mp3
   ├── advert
       ├── 0001.mp3
       ├── ...
       ├── 0255.mp3
   ├── mp3
       ├── 0001.mp3
       ├── ...
       ├── 0255.mp3
       ├── 0300_new_tag.mp3 
       ├── 0310_tag_linked.mp3 
       ├── 0311_mode_random_episode.mp3 
       ├── 0312_mode_album.mp3 
       ├── 0313_mode_party.mp3 
       ├── 0314_mode_single_track.mp3 
       ├── 0315_mode_audio_book.mp3 
       ├── 0316_admin.mp3 
       ├── 0320_select_file.mp3 
       ├── 0330_say_number_ask.mp3 
       ├── 0331_say_number_no.mp3 
       ├── 0332_say_number_yes.mp3 
       ├── 0400_tag_ok.mp3 
       ├── 0401_error.mp3 
       ├── 0800_reset_tag.mp3 
       ├── 0801_reset_tag_ok.mp3 
       ├── 0802_reset_aborted.mp3 
       ├── 0999_reset_ok.mp3 
```

Details can be found in the [data sheet](https://github.com/DFRobot/DFRobotDFPlayerMini/blob/master/doc/FN-M16P%2BEmbedded%2BMP3%2BAudio%2BModule%2BDatasheet.pdf) under *3.6.5 Specify playback of a track in a folder*.
