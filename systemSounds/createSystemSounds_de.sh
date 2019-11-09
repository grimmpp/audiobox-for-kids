#!/bin/bash

# docs: https://pypi.org/project/google-speech/
# original script: https://github.com/xfjx/TonUINO/blob/master/create-soundfiles.sh

lang="de"

echo "Generate number sound files: 1-255"
for i in {1..255};
do
    j=$(printf "%04d" $i)
    cmd="google_speech -l $lang \"$i\" -o $j.mp3"
    echo $cmd
    $($cmd)
done


text="Oh, eine neue Karte! Verwende die Lautstärke Tasten um einen Ordner für die Karte auszuwählen. Drücke die Pause Taste um die Auswahl zu speichern."
file="0300_new_tag"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="OK, ich habe die Karte mit dem Ordner verknüpft. Wähle nun mit den Lautstärke Tasten den Wiedergabemodus aus."
file="0310_tag_linked"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Hörspielmodus: Eine zufällige Datei aus dem Ordner wiedergeben"
file="0311_mode_random_episode"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Albummodus: Den kompletten Ordner wiedergeben"
file="0312_mode_album"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Party Modus: Ordner zufällig wiedergeben."
file="0313_mode_party"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Einzel Modus: Eine bestimmte Datei im Ordner wiedergeben."
file="0314_mode_single_track"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Hörbuch Modus: Einen Ordner wiedergeben und den Fortschritt speichern."
file="0315_mode_audio_book"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Admin Funktionen."
file="0316_admin"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="OK, wähle nun bitte die Datei mit den Lautstärke Tasten aus. "
file="0320_select_file"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Soll ich vor einer Datei jeweils die Nummer ansagen? Du kannst jederzeit durch einen langen Druck auf die Pause Taste die aktuelle Nummer abfragen." 
file="0330_say_number_ask"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Nein, Nummer nicht ansagen."
file="0331_say_number_no"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Ja, Nummer ansagen."
file="0332_say_number_yes"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="OK. Ich habe die Karte konfiguriert." 
file="0400_tag_ok"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Oh weh! Das hat leider nicht geklappt!." 
file="0401_error"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Bitte lege die zu löschende Karte auf! Zum Abbrechen einfach eine der Lautstärke Tasten drücken!"
file="0800_reset_tag"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="OK, du kannst den Tag nun wieder neu konfigurieren." 
file="0801_reset_tag_ok"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="OK, ich habe den Vorgang abgebrochen." 
file="0802_reset_aborted"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd

text="Reset wurde durchgeführt!" 
file="0999_reset_ok"
cmd="google_speech -l $lang \"$text\" -o \"$file.mp3\""
echo $cmd
eval $cmd
