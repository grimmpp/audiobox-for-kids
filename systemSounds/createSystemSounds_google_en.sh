#!/bin/bash

# docs: https://pypi.org/project/google-speech/
# original script: https://github.com/xfjx/TonUINO/blob/master/create-soundfiles.sh

source helper.sh
source createNumberMp3s_google.sh

lang="en"
dirMp3="goolge/$lang/mp3"
dirAdvert="goolge/$lang/advert"

CreateFolders $dirMp3 $dirAdvert

CopyStaticFiles $dirMp3

GenerateNumbers $lang $dirMp3 $dirAdvert

declare -a text=(
    "0300_new_tag"              "Oh, a new card. Use the volume buttons to choose a folder and then use the pause button to store it."
    "0310_tag_linked"           "OK, I just created a connection between the selected folder and your card. Now, choose an audio mode by using the volume buttons."
    "0311_mode_random_episode"  "Audio Book: Plays a random track from the selected folder."
    "0312_mode_album"           "Album Mode: Plays all tracks within the selected folder."
    "0313_mode_party"           "Party Mode: Plays tracks randomly."
    "0314_mode_single_track"    "Single Mode: Plays only one specific track."
    "0315_mode_audio_book"      "Audo Book with bookmark: Plays all tracks from the selected folder and stores the progress."
    "0316_admin"                "Admin function."
    "0320_select_file"          "OK, now choose the track by using the volume buttons."
    "0330_say_number_ask"       "Shall I say the numer of the track? You can always check the number of a track by pushing the puase button for a second."
    "0331_say_number_no"        "No, I won't tell you the track numbers."
    "0332_say_number_yes"       "Yes, I will tell you the track numbers."
    "0400_tag_ok"               "OK. I completed the configutation of the card."
    "0401_error"                "Oh! An error occourred!." 
    "0800_reset_tag"            "Please, put the card you want to delete on top of the box! You can cancel this by pushing one of the volume buttons!"
    "0801_reset_tag_ok"         "OK, now you can reconfigure the card."
    "0802_reset_aborted"        "OK, I cancelled this process." 
    "0999_reset_ok"             "Reset was completed!" 
);

echo -e "\n\e[32mGenerate Menu files:\e[39m"

max=$(( ${#text[@]} / 2 - 1))
for (( i=0; i<=$max; i++ ))
do 
    cmd="google_speech -l $lang \"${text[2*i+1]}\" -o \"$dirMp3/${text[2*i]}.mp3\""
    echo $cmd
    eval $cmd
done
