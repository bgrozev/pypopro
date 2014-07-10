#!/bin/sh

libs=

cc="gcc -O2 -Wall"
ffmpeg_src="/Users/boris/jitsi/src/ffmpeg"

#$cc -I$ffmpeg_src -c -o decoder.o decoder.c
#$cc -I$ffmpeg_src -c -o encoder.o encoder.c
#$cc -I$ffmpeg_src -c -o overlay.o overlay.c

ll="-L/Users/boris/jitsi/src/ffmpeg/libswresample -L/Users/boris/jitsi/src/ffmpeg/libavcodec -L/Users/boris/jitsi/src/ffmpeg/libavformat -L/Users/boris/jitsi/src/ffmpeg/libavutil -L/Users/boris/jitsi/src/libvpx -L/Users/boris/jitsi/src/ffmpeg/libavfilter -L/Users/boris/jitsi/src/ffmpeg/libswscale -lavcodec -lavformat -lvpx -lavutil -lavfilter -lswscale -lz -lm -lbz2 -lswresample -framework CoreFoundation -framework CoreVideo -framework VideoDecodeAcceleration"

#$cc -I$ffmpeg_src -c -o test.o test.c
#$cc -c ivf.c -o ivf.o
#$cc test.o encoder.o decoder.o overlay.o ivf.o $ll -o test


$cc -I$ffmpeg_src pypopro.c -c -o pypopro.o -I/opt/local/Library/Frameworks/Python.framework/Versions/3.4/include/python3.4m
$cc -shared pypopro.o encoder.o decoder.o overlay.o ivf.o $ll -L/opt/local/Library/Frameworks/Python.framework/Versions/3.4/lib/ -lpython3.4 -o pypopro.so
