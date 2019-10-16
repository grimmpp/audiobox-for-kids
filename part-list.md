# Parts
* **Arduino Nano** with ATmega328 (old bootloader in this case) (See Starter Kit for TonUINO)
  Depending on wich version you buy you need to configure your project/IDE so that the bootloader is compatible. New versions have a better and faster bootloader. 
* **DFMiniPlayer** (See Starter Kit for TonUINO) <br />
  (There are a lot of modules with different controllers/decoders. Ensure you buy one with MH-ET LIVE (MH2024K-24SS) e.g. [this one](https://www.ebay.de/itm/Arduino-WeMos-MP3-Player-Decoder-Modul-Card-Reader-MH2024K-DFPlayer-UART-3-3-5V/283613104099?hash=item4208a887e3:g:7P8AAOSw6-Nde04L). This one has a really good sound quality and runs stable. I really had problems with other decoders which made white noice and did not work stable. [Here](http://discourse.voss.earth/t/dfplayer-verschiedene-versionen/681/13) is also a deeper discussion and experiences about the decoders.)
* **RFID Reader RC522** (See Starter Kit for TonUINO)
* Electrolytic **capacitor 100µF** for voltage stabilization
* Film **capacitor 100nF** for voltage stabilization
* **Resistors** 3x1K, 0,1K for UART communication btw Arduino and DfMiniPlayer and for power bank wake up logic
* **Transistor** for power bank wake up logic
* **Speaker** ([FR 10 HM - 4 Ohm](https://www.visaton.de/de/produkte/chassiszubehoer/breitband-systeme/fr-10-hm-4-ohm)) [Amazon Offer](https://www.amazon.de/Visaton-schwarz-Lautsprecher-22000-Hz-kabelgebunden/dp/B003A63NLE/ref=sr_1_3?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&keywords=tonuino&qid=1571168322&sr=8-3)
* **3 Buttons** (Next/VolUp, Play/Pause, Prev/VolDown) [Amazon Offer](https://www.amazon.de/gp/product/B076SK1HMM/ref=ppx_yo_dt_b_asin_title_o03_s00?ie=UTF8&psc=1)
* **Power Switch**
* **Power Jack**
* **External Power Adapter** (5V Charger)
* **Power Bank** as battery [Amazon Offer](https://www.amazon.de/gp/product/B075ZNG92W/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1) <br />
  Power Banks have a smart logic which switches the power bank off after about 30sec if the load is too small. Therefore I build a logic to keep it alive.
* **Cable for wiring**
* **Perfboard** <br />
  You can actually also buy the [board](https://discourse.voss.earth/t/neue-platine-version-1-51/63) designed by Thorsten Voß. The board from Thorsten Voß unfortunately does not contain a logic to keep the power bank alive.
* Screws and glue for mounting all parts
* **Case** <br />
  Let you get inspired by this nice [posts](https://discourse.voss.earth/t/tonuino-gehaeuse-galerie/786).
* **TF Card** (max. 32GB) for storing the mp3 files.
* **RFID Keycard MF S50 13,56 MHz** for selecting the tracks. [Amazon Offer](https://www.amazon.de/gp/product/B07TVJPTM7/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)
* **NFC Tags 13,56 MHz** for selecting the tracks. [Amazon Offer](https://www.amazon.de/gp/product/B077JTT3NQ/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1)
* **NFC Stickers NTAG 215 13,56 MHz** for selecting the tracks [Amazon Offer](https://www.amazon.de/gp/product/B06Y1BLLD4/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)

**Starter Kit TonUINO** on [Amazon Offer](https://www.amazon.de/AZDelivery-TonUINO-Arduino-Player-Karten/dp/B07JLBGYQ6/ref=sxin_5_ac_d_rm?__mk_de_DE=%C3%85M%C3%85%C5%BD%C3%95%C3%91&ac_md=0-0-dG9udWlubw%3D%3D-ac_d_rm&keywords=tonuino&pd_rd_i=B07JLBGYQ6&pd_rd_r=3e28fc6e-1ba6-410d-b2b4-171652f702df&pd_rd_w=IqVSC&pd_rd_wg=WVzju&pf_rd_p=1307f5d7-7a93-4246-82b9-71e02e065a30&pf_rd_r=866KF5YW2B88GGM4TGM5&psc=1&qid=1571168562)

# Software
* is contained in this git repo.

# Hardware Tooling
* Ususal stuff like wire cutter, screw driver, ...
* Soldering station
* Depending what kind of case you build: saw, drill, 3D-printer, ...

# Software Tooling
* [Platformio (pio)](https://docs.platformio.org/en/latest/ide/vscode.html) (required for building and pushing code to arduino)
* [VSCode](https://code.visualstudio.com/) (optional only required for chaning code) 
* docker (optional for running platformio in docker container / portable version)
