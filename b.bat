clear
arm-none-eabi-gcc -O2 -I C:\devkitPro\libgba\include -g -o game.elf -specs=gba.specs main.c draw.c lut.c emath.c blocks.c player.c raycast.c asmdraw.S
arm-none-eabi-objcopy -O binary game.elf game.gba
gbafix game.gba
