## Inspiration

SpriteSaver is primarily designed to display Pokémon sprites. In Generation 5, Nintendo introduced animated battle sprites for all Pokémon. These are the source of the examples shown in the gallery.

## Downloading Sprites

There are many online sources for downloading animated Pokémon sprites. Here are a few good ones,

- [Bulbagarden: Black 2 and White 2 sprites](https://archives.bulbagarden.net/wiki/Category:Black_2_and_White_2_sprites) - Browse animated sprites by Pokédex number
- [Pokémon Database: Pokémon sprite archive](https://pokemondb.net/sprites) - Download front and back GIFs
- [Project Pokémon: Sprite Index](https://projectpokemon.org/home/docs/spriteindex_148/) - Browse 3D sprites (front idle animation only)

## Tutorial

For this example, I'll use the [Aerodactyl](https://pokemondb.net/sprites/aerodactyl) sprite seen in the gallery.

First, place the front and back GIFs in a folder inside `sprites`,
```
spritesaver/
├── autobuild.sh
├── configure.sh
├── Dockerfile
├── sprites/
│   ├── +aerodactyl/
│   │   ├── +aerodactyl_back.gif
│   │   └── +aerodactyl_front.gif
│   ├── Makefile
│   └── spritesaver.c
└── spritesaver.xml
```

Then, run `autobuild.sh` from the `spritesaver` directory,
```
sudo ./autobuild.sh sprites/aerodactyl/aerodactyl_front.gif sprites/aerodactyl/aerodactyl_back.gif
```

This will generate the sprite images, sprite headers, and compile and install the binary,
```
spritesaver/
├── autobuild.sh
├── configure.sh
├── Dockerfile
├── sprites/
│   ├── aerodactyl/
│   │   ├── aerodactyl_back.gif
│   │   ├── aerodactyl_front.gif
│   │   ├── +sprite_headers/
│   │   │   ├── +aerodactyl_back_01_png.h
│   │   │   └── ...
│   │   ├── +sprite_images/
│   │   │   ├── +aerodactyl_back_01.png
│   │   │   └── ...
│   │   └── +sprites.h
│   ├── Makefile
│   ├── +spritesaver
│   └── spritesaver.c
└── spritesaver.xml
```

To preview the screensaver, either use the XScreenSaver GUI or run the binary directly,
```
./spritesaver --speed [1-10] --animation-delay [1-5]
```

For more documentation on the settings, see [SETTINGS.md](SETTINGS.md).
