Knock detector program that utilizes dynamic time warping in order to compare to passwords recorded by a button on a telos b mote.

## Build commands ##
Make and upload telos b
```
  make TARGET=sky MOTES=/dev/ttyUSB0 knockDetector.upload login
```
 Check serial port:
```
make TARGET=sky motelist
```
compile and upload program
```
make TARGET=sky PORT=/dev/ttyUSB0 knockDetector.upload
```
ssh on the sensor
```
make TARGET=sky PORT=/dev/ttyUSB0 login
```
Clean build
```
make TARGET=sky distclean
```
local sky build

```
 make TARGET=sky
```
