# Zatacka X
Zatacka X is a modern, free remake of the classic Achtung, die Kurve!. In
addition to all the features from the original game, Zatacka X includes
special weapons, updated graphics, sound effects and AI controlled
opponents.

![screenshot](http://folk.uio.no/simenheg/zatackax_small.png)

## Installation
To compile Zatacka X, run:

    $ ./configure
    $ make

Compilation depends on the following libraries:

* SDL 1.2
* SDL_image 1.2
* SDL_ttf 2.0
* SDL_mixer 1.2

All of which are found on the SDL website: http://www.libsdl.org/.

These libraries are probably provided by your GNU/Linux
distribution. Under Debian, for instance, you could run the following
command to install all the required libraries:

    $ sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev \
        libsdl-ttf2.0-dev libsdl-mixer1.2-dev

## License
The Zatacka X code is free software, licensed under the GNU General Public
License version 3, or (at your option) any later version. See COPYING for
details.

All other content (graphics, audio) are licensed under Creative Commons
Attribution-ShareAlike (CC BY-SA), unless otherwise noted in the AUTHORS
file.

## Credits
See AUTHORS.

## Contact
Questions, suggestions, contributions or other inquiries? Please contact
simenheg@gmail.com.
