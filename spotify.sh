#!/bin/bash

PLAYER="spotify"

GETPLAYER="dbus-send --print-reply \
--dest=org.freedesktop.DBus \
/org/freedesktop/DBus \
org.freedesktop.DBus.ListNames \
| grep mpris \
| grep $PLAYER \
| sed 's/.*string \"//g;s/.$//g'"

DEST=$(eval $GETPLAYER)
echo $DEST

ARTISTCOMMAND="dbus-send --print-reply \
--dest="$DEST" \
/org/mpris/MediaPlayer2 \
org.freedesktop.DBus.Properties.Get \
string:"org.mpris.MediaPlayer2.Player" \
string:"Metadata" \
| grep -A 2 'artist' \
| tail -1 \
| sed 's/.*string \"//g;s/.$//g'"

TITLECOMMAND="dbus-send --print-reply \
--dest="$DEST" \
/org/mpris/MediaPlayer2 \
org.freedesktop.DBus.Properties.Get \
string:"org.mpris.MediaPlayer2.Player" \
string:"Metadata" \
| grep -A 1 'title' \
| tail -1 \
| sed 's/.*string \"//g;s/.$//g'"

# echo $ARTISTCOMMAND
# echo $TITLECOMMAND

ARTIST=$(eval $ARTISTCOMMAND)
TITLE=$(eval $TITLECOMMAND)

echo $ARTIST
echo $TITLE

STATUSCOMMAND="dbus-send --print-reply \
--dest="$DEST" \
/org/mpris/MediaPlayer2 \
org.freedesktop.DBus.Properties.Get \
string:org.mpris.MediaPlayer2.Player \
string:PlaybackStatus \
| grep variant \
| sed 's/.*string \"//g;s/.$//g'"

STATUS=$(eval $STATUSCOMMAND)
echo $STATUS

# cscroll -l 25 -d 0.1 -u 5 -c "$ARTISTCOMMAND" " - " -c "$TITLECOMMAND" -s ""