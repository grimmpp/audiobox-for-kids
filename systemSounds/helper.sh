#!/bin/bash 

function CreateFolders {
    dirMp3=$1
    dirAdvert=$2

    echo -e "\n\e[32mCreate directories:\e[39m"
    echo "mkdir -p $dirMp3"
    mkdir -p $dirMp3

    echo "mkdir -p $dirAdvert"
    mkdir -p $dirAdvert
}

function CopyStaticFiles {
    dirMp3=$1

    echo -e "\n\e[32mCopy static mp3-files:\e[39m"
    cmd="cp staticMp3s/*.mp3 $dirMp3"
    echo $cmd
    $($cmd)
}