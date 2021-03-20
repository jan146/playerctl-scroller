#!/bin/bash

# Name of your polybar bar, which will
# run this script. The default bar is
# named "example".
BAR="example"

# Set the name of the module, which
# contains the play/pause button.
# The module should be located in the
# bar that is defined above this.
# Leave blank to disable play/pause
# button functionality.
# MODULE=""
MODULE="spotify-play-pause"

# Set the name of desired mpris player,
# such as firefox, spotify, chromium,
# vlc or simply just playerctl.
PLAYER="spotify"

# Set delay (in seconds) between rotating 
# a single character of text (lower
# number correspons to faster scrolling).
DELAY="0.2"

# Set the maximum length of the text.
# If the text is longer than LENGTH,
# it will rotate, otherwise it will not.
LENGTH="30"

# If force is set to "1", the text
# will rotate, even if it is not
# too long.
# FORCE="1"
FORCE="0"

# Set a separator for the text.
# If the text should rotate, then
# this string will be appended to
# the end of the text.
SEPARATOR="  "

# Set text between the artist and
# title texts.
MIDDLE=" — "

# The text will be updated every INTERVAL
# rotations, i. e. if DELAY is set to 0.2
# and INTERVAL is set to 5, then the
# status and text will be updated every
# 0.2 * 5 = 1 second(s). This is mainly
# meant to reduce an unnecessary high
# number of dbus requests and minimize
# resource usage.
INTERVAL="5"

# If you have the i3 workspaces module,
# you can set the amount of characters
# that this module will shrink by
# for every open workspace
i3="0"

### END OF USER CONFIGURATION ###

if [ "$1" = "--update" ]; then

    DEST=$2

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

    ARTIST=$(eval $ARTISTCOMMAND)
    TITLE=$(eval $TITLECOMMAND)
    
    if [[ -n "$ARTIST" ]]; then
        echo "$ARTIST$MIDDLE$TITLE" ; exit 0
    else
        echo "$TITLE" ; exit 0
    fi

fi

if [ "$1" = "--status" ]; then

    GETDEST="dbus-send --print-reply \
    --dest=org.freedesktop.DBus \
    /org/freedesktop/DBus \
    org.freedesktop.DBus.ListNames \
    | grep mpris \
    | grep $PLAYER \
    | sed 's/.*string \"//g;s/.$//g'"

    DEST=$(eval $GETDEST)

    if [[ -n "$DEST" ]]; then

        STATUSCOMMAND="dbus-send --print-reply \
        --dest="$DEST" \
        /org/mpris/MediaPlayer2 \
        org.freedesktop.DBus.Properties.Get \
        string:org.mpris.MediaPlayer2.Player \
        string:PlaybackStatus \
        | grep variant \
        | sed 's/.*string \"//g;s/.$//g'"

        STATUS=$(eval "$STATUSCOMMAND" 2> /dev/null)
        
        if [[ -n "$STATUS" ]]; then
            echo "$STATUS $DEST" ; exit 0
        else
            echo "OFFLINE" ; exit 0
        fi

    else
        echo "OFFLINE" ; exit 0
    fi

fi

PID=$(pgrep -a "polybar" | grep "$BAR" | cut -d" " -f1)
DIR="$(dirname "$(readlink -f "$0")")"

RUNCOMMAND="playerctl-scroller \
    -l $LENGTH \
    -d $DELAY -u $INTERVAL \
    -i $PID \
    -p $PLAYER \
    -m \"$MODULE\" \
    -r \"$DIR/scroller.sh\" \
    -s \"$SEPARATOR\""

if [ $FORCE = "1" ]; then
    RUNCOMMAND="$RUNCOMMAND -f"
fi
if [ $i3 -gt "0" ]; then
    RUNCOMMAND="$RUNCOMMAND -3 $i3"
fi

eval $RUNCOMMAND