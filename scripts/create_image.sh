#!/usr/bin/env bash

if (( $# != 1 ))
then
  echo "Usage: bash create_image.sh <filename>"
  exit 0
fi

ffmpeg -i ${1} -pix_fmt yuv422p out.yuv

