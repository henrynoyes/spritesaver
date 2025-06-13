## Overview

SpriteSaver offers customization through settings. These can be set in the XScreenSaver GUI or passed in as arguments when running the binary,
```
./spritesaver --speed [1-10] --animation-delay [1-5]
```

- [Speed](#speed)
- [Animation Delay](#animation-delay)

## Speed

The movement speed of the sprite in pixels per frame. Ranges from 1 (slow) to 10 (fast). Defaults to 3.

| 3 pixels/second | 6 pixels/second |
|:---------------:|:---------------:|
| ![3 speed](gallery/settings-comparison/aerodactyl_3speed_2delay.gif) | ![6 speed](gallery/settings-comparison/aerodactyl_6speed_2delay.gif) |

## Animation Delay

The refresh delay of the sprite animation in frames. Ranges from 1 (fast) to 5 (slow). Defaults to 2.

| 2 frame delay | 1 frame delay |
|:-------------:|:-------------:|
| ![3 speed](gallery/settings-comparison/aerodactyl_3speed_2delay.gif) | ![6 speed](gallery/settings-comparison/aerodactyl_3speed_1delay.gif) |