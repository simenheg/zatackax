## GNU/LINUX/BSD/UNIX-LIKE
To compile Zatacka X, simply run:

    $ make

Compilation depends on the following libraries:

* SDL
* SDL_image
* SDL_ttf
* SDL_mixer

All of which are found on the SDL website: http://www.libsdl.org/.

These libraries are probably also provided by your
GNU/Linux/BSD-distribution. Under Debian, for instance, you could run the
following command to install all the required libraries:

    $ sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev \
        libsdl-ttf2.0-dev libsdl-mixer1.2-dev

## MICROSOFT WINDOWS
The following library files are needed to compile with Mingw32.

### SDL-developer-mingw32
Download SDL-devel-1.2.14-mingw32 from
http://www.libsdl.org/release/SDL-devel-1.2.14-mingw32.tar.gz. Extract the
content in lib\ into \masm32\lib and extract the content in include\ into
\masm32\include (make sure the SDL folder is included in the
extraction). You should end up with \masm32\include\SDL\ and several header
files within that subfolder.
    
### SDL_tff
Download SDL_ttf-devel-2.0.9 from
http://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-devel-2.0.9-VC8.zip.
Extract the content in lib\*.lib into \masm32\lib and extract the dll files
into the zatackax\ folder. Extract the include\ content into
\masm32\include\SDL.
   
### SDL_image
Download SDL_image-devel-1.2.10 from
http://www.libsdl.org/projects/SDL_image/release/SDL_image-devel-1.2.10-VC.zip.
Extract the content in lib\*.lib into \masm32\lib and extract the dll files
into the zatackax\ folder. Extract the include\ content into
\masm32\include\SDL.
    
### SDL_mixer
Download SDL_mixer-devel-1.2.11 from
http://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-devel-1.2.11-VC.zip.
Extract the content in lib\*.lib into \masm32\lib and extract the dll files
into the zatackax\ folder. Extract the include\ content into
\masm32\include\SDL.

### DLLs needed to compile (either in folder or linked to):
* jpeg.dll
* libfreetype*6.dll
* libogg*0.dll
* libpng12*0.dll
* libvorbis*0.dll
* libvorbisfile*3.dll 
* SDL.dll
* SDL_image.dll
* SDL_mixer.dll
* SDL_ttf.dll
* zlib1.dll
