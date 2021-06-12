# playerctl-scroller

Scroll the currently playing song via playerctl. It is mainly meant to be used with <a href="https://github.com/polybar/polybar">polybar</a>.<br>
Backend is based on <a href="https://github.com/jan146/Cscroll">Cscroll</a>.<br>
Frontend is based on <a href="https://github.com/PrayagS/polybar-spotify">polybar-spotify</a>.<br>
All credit for the idea goes to the original creator.

## Installation

Before installing, make sure you have installed all the required dependencies (listed below).

```bash
git clone https://github.com/jan146/playerctl-scroller.git
cd playerctl-scroller
sudo make
make config
```

## Configuration

After installation, add the following modules to the config in `~/.config/polybar/config`:

```ini

[module/spotify]
type = custom/script
tail = true
format = <label>
exec = $HOME/.config/polybar/scripts/scroller.sh

[module/spotify-play-pause]
type = custom/ipc
hook-0 = echo "  "
hook-1 = echo "  "
initial = 1
click-left = playerctl -p spotify play-pause

[module/spotify-prev]
type = custom/text
content = " "
click-left = playerctl -p spotify previous

[module/spotify-next]
type = custom/text
content = ""
click-left = playerctl -p spotify next
```

In order to change the play-pause button on-click, ipc has to be enabled.<br>
This can be done by adding `enable-ipc=true` to where your bar is defined.<br>
Additionally, you will also want to add these modules to the bar. The end result should look something like this:

```ini
[bar/example]
enable-ipc=true
modules-left = spotify spotify-prev spotify-play-pause spotify-next
; other bar settings
```

Now you can configure the `scroller.sh` file, located in `~/.config/polybar/scripts`.
There, you can adjust numerous parameters, such as the delay between scrolling a single character (i. e. scrolling speed), update frequency, player, text format, length of the text ...

## Switch player

If you want to switch to, say `vlc` as your preffered media player, you have to do the following:<br>
In the polybar modules (`~/.config/polybar/config`), change the `click-left` options. For instance:

```ini
click-left = playerctl -p spotify play-pause
click-left = playerctl -p spotify previous
click-left = playerctl -p spotify next
```
to<br>
```ini
click-left = playerctl -p vlc play-pause
click-left = playerctl -p vlc previous
click-left = playerctl -p vlc next
```

Also, you have to edit the `scroller.sh` file:

```ini
PLAYER="spotify"
```
to
```ini
PLAYER="vlc"
```

You can view all the available (currently active) players by running `playerctl -l`.<br>

## Default player

There is also the `playerctl` option, which will use the player that was last active.<br>
This can be enabled by omitting the `-p` parameter:

```ini
click-left = playerctl play-pause
click-left = playerctl previous
click-left = playerctl next
```

and editing the `scroller.sh` file:

```ini
PLAYER="playerctl"
```

## Why

All credit for the idea goes to the <a href="https://github.com/PrayagS/polybar-spotify">original creator</a>.

This solution is a bit more optimized, since it communicates directly with the dbus interface, instead of using playerctl.
Along with other performance optimizations, this solution yields much lower cpu usage, without making any significant sacrifices, which was my original goal. 

## Dependencies
* <a href="https://github.com/git-guides/install-git">git</a> (to clone the repo)
* <a href="https://gcc.gnu.org/releases.html">gcc</a> (C compiler)
* base-devel
* <a href="https://github.com/freedesktop/dbus">dbus</a>
* <a href="https://github.com/altdesktop/playerctl">playerctl</a> (only for button functionality)
<br>
All of the above should be available in your built-in package manager's repositories (preffered way).
