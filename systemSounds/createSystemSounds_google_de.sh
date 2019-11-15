#!/bin/bash

# docs: https://pypi.org/project/google-speech/
# original script: https://github.com/xfjx/TonUINO/blob/master/create-soundfiles.sh

source helper.sh
source createNumberMp3s_google.sh

lang="de"
dirMp3="goolge/$lang/mp3"
dirAdvert="goolge/$lang/advert"

CreateFolders $dirMp3 $dirAdvert

CopyStaticFiles $dirMp3

GenerateNumbers $lang $dirMp3 $dirAdvert

declare -a text=(
    "0300_new_tag"              "Oh, eine neue Karte! Verwende die Lautstärke Tasten um einen Ordner für die Karte auszuwählen. Drücke die Pause Taste um die Auswahl zu speichern."
    "0310_tag_linked"           "OK, ich habe die Karte mit dem Ordner verknüpft. Wähle nun mit den Lautstärke Tasten den Wiedergabemodus aus."
    "0311_mode_random_episode"  "Hörspielmodus: Eine zufällige Datei aus dem Ordner wiedergeben"
    "0312_mode_album"           "Albummodus: Den kompletten Ordner wiedergeben"
    "0313_mode_party"           "Party Modus: Ordner zufällig wiedergeben."
    "0314_mode_single_track"    "Einzel Modus: Eine bestimmte Datei im Ordner wiedergeben."
    "0315_mode_audio_book"      "Hörbuch Modus: Einen Ordner wiedergeben und den Fortschritt speichern."
    "0316_admin"                "Admin Funktionen."
    "0320_select_file"          "OK, wähle nun bitte die Datei mit den Lautstärke Tasten aus."
    "0330_say_number_ask"       "Soll ich vor einer Datei jeweils die Nummer ansagen? Du kannst jederzeit durch einen langen Druck auf die Pause Taste die aktuelle Nummer abfragen." 
    "0331_say_number_no"        "Nein, Nummer nicht ansagen."
    "0332_say_number_yes"       "Ja, Nummer ansagen."
    "0400_tag_ok"               "OK. Ich habe die Karte konfiguriert."
    "0401_error"                "Oh weh! Das hat leider nicht geklappt!." 
    "0800_reset_tag"            "Bitte lege die zu löschende Karte auf! Zum Abbrechen einfach eine der Lautstärke Tasten drücken!"
    "0801_reset_tag_ok"         "OK, du kannst den Tag nun wieder neu konfigurieren."
    "0802_reset_aborted"        "OK, ich habe den Vorgang abgebrochen." 
    "0999_reset_ok"             "Reset wurde durchgeführt!" 
);

echo -e "\n\e[32mGenerate Menu files:\e[39m"

max=$(( ${#text[@]} / 2 - 1))
for (( i=0; i<=$max; i++ ))
do 
    cmd="google_speech -l $lang \"${text[2*i+1]}\" -o \"$dirMp3/${text[2*i]}.mp3\""
    echo $cmd
    eval $cmd
done
