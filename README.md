# AudioBox for Kids (Arduino and DFMiniPlay Project)
This project is about an audiobox which allows small kids, who cannot read, don't understand signs, or don't know which button to push, an easy way to choose sound effects, audio books, or music tracks. This can be done by putting small items on the top of the audiobox. Those items are related to the e.g. audio book they want to play and can be e.g. a small figure or a labelled cards or tags. 
<br />
<img src="/pics/AudioboxForKids.png" width=300/>

# What can it do?
...

# Hardware
* **[Part List](/part-list.md)** <br />
<img src="/electronics/Schematic.png" height=200 />

# Arduino Software
## PlatformIO commands:
* Build code: `pio run`
* Build and upload code: `pio run -v -t upload`
* Open serial monitor: `pio device monitor -b 115200`

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
