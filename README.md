# Chipper No. 8
## What is it
**Chipper No. 8** is my first attempt of emulation.  
Many people suggest to choose CHIP-8 as your first target system, so I did exactly that.  
It's a basic CHIP-8 emulator without any support for Super CHIP.

## Compilation and usage
Install `cmake` and `SDL2` and run `./build.sh`.  
Or do it manually:  
```
mkdir -p build
cd build
cmake ..
make
```

After that run the executable: ``./chipper8 <romfile>``

## Buttom mapping
The button mapping is currently a bit weird:
```
---------       ---------
|1|2|3|4|       |1|2|3|C|
|Q|W|E|R|   ->  |4|5|6|D|
|A|S|D|F|   ->  |7|8|9|E|
|X|C|V|B|       |A|0|B|F|
---------       ---------
```


## TODO:
+ Fix flickering and disappearing graphics
+ Emulate timer properly
+ Possibly enable custom button mapptings