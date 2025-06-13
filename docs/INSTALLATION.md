# Installation

> [!NOTE] 
> This software was developed and tested on **Ubuntu 22.04**. Support for other platforms is listed in [Future Features](#future-features).

First, install XScreenSaver and [FFmpeg](https://ffmpeg.org/) if they are not installed already,
```
sudo apt install xscreensaver ffmpeg
```

Then clone and run `configure.sh`. This **only needs to be run once** as long as XScreenSaver is not uninstalled.
```
git clone git@github.com:henrynoyes/spritesaver.git
cd spritesaver && sudo ./configure.sh
```

### Importing sprites

Find a pair of sprites in GIF format and place them in a new directory inside `sprites/`. Make sure both GIFs contain the **same number of frames**.

For detailed instructions on installing Pokémon sprites, see [POKEMON.md](docs/POKEMON.md).

### Installation Methods

- [Autobuild with Docker](#autobuild-with-docker)
- [Manually build with Docker](#manually-build-with-docker)
- [Manually build from source](#manually-build-from-source)
- [Previewing](#previewing)

## Autobuild with Docker

To use `autobuild.sh`, make sure you have a working Docker installation. Then simply run,
```
sudo ./autobuild.sh sprites/sprite_name/left.gif sprites/sprite_name/right.gif
```

Open the XScreenSaver GUI by launching the `Screensaver` application or running the `xscreensaver-demo` command. Select SpriteSaver and modify the desired [settings](docs/SETTINGS.md).

To preview SpriteSaver, see [Previewing](#previewing).

## Manually build with Docker

Convert your GIFs,
```
./convert_gifs.sh /sprites/sprite_name/left.gif sprites/sprite_name/right.gif
```

Build the `xscreensaver-build` image,
```
docker build -t xscreensaver-build .
```

Enter a container and compile,
```
docker run --rm -it -v $(pwd)/sprites:/xscreensaver/hacks/sprites xscreensaver-build
make -s -C hacks/sprites sprite_dir=sprite_name
```

Exit the container and install the binary,
```
sudo chown -R $SUDO_UID:$SUDO_GID sprites
sudo cp sprites/spritesaver /usr/libexec/xscreensaver
```

Restart XScreenSaver and select SpriteSaver,
```
xscreensaver-command -restart
xscreensaver-demo # select SpriteSaver in GUI
```

To preview SpriteSaver, see [Previewing](#previewing).

## Manually build from source

Install build dependencies,
```
sudo apt update && apt install -y \
    perl pkg-config gettext intltool \
    libx11-dev libxext-dev libxi-dev libxt-dev libxft-dev \
    libxinerama-dev libxrandr-dev libxxf86vm-dev \
    libgl-dev libglu1-mesa-dev libglew-dev \
    libgtk-3-dev libgdk-pixbuf2.0-dev \
    libjpeg-dev libxml2-dev libpam0g-dev \
    libsystemd-dev
```

Clone the XScreenSaver source code and configure,
```
git clone https://github.com/Zygo/xscreensaver.git
cd xscreensaver && export XSS_DIR=$(pwd)
./configure
```

Convert your GIFs,
```
cd spritesaver # wherever it was cloned
./convert_gifs.sh /sprites/sprite_name/left.gif sprites/sprite_name/right.gif
```

Symlink the sprites directory,
```
ln -s sprites $XSS_DIR/hacks/sprites
```

Compile with the desired `sprite_name`,
```
make -s -C $XSS_DIR/hacks/sprites sprite_dir=sprite_name
```

Install the binary,
```
sudo cp sprites/spritesaver /usr/libexec/xscreensaver
```

Restart XScreenSaver and select SpriteSaver,
```
xscreensaver-command -restart
xscreensaver-demo # select SpriteSaver in GUI
```

To preview SpriteSaver, see [Previewing](#previewing).

## Previewing

To preview SpriteSaver, you can manually activate XScreenSaver with,
```
xscreensaver-command -activate
```

It can also be previewed in a floating window by running the binary,
```
./sprites/spritesaver
```