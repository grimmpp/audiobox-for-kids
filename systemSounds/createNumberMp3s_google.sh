#!/bin/bash 

function GenerateNumbers {
    lang=$1
    dirMp3=$2
    dirAdvert=$3

    echo -e "\n\e[32mGenerate number sound files: 1-255\e[39m"
    for i in {1..255};
    do
        # Generate text to mp3
        j=$(printf "%04d" $i)
        cmd="google_speech -l $lang \"$i\" -o $dirMp3/$j.mp3"
        echo $cmd
        $($cmd)
        
        # Copy same file to advert folder
        cmd="cp $dirMp3/$j.mp3 $dirAdvert/$j.mp3"
        echo $cmd
        $($cmd)
    done

}