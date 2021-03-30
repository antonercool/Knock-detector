## Knock Detector ##
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


## Password Recording ##
*Secret password recording*
* At program start a led indicater will blink 3 times.
* When all Red,Green & Blue led is on the recording is active (10 sec)
* After the recording period, the user ends with at button press.

*login recording*
* The mote will be locked, indicated by the Red led.
* When the user presses a sequence of button clicks the Blue led will indicate (login recording)
* If the password is acceptable with ACCEPT_THRESHOLD then the Green led will indicate succes
* If not, the Red led with indicate locked, and the user is able to retry the login sequence

*storage logic*

* For each password the user has 10 sec(1280 clock ticks) to input presses (Maximum 13 presses)
* Each press gets stored within a time slot(128 ticks/1sec)   array of size [13]


```
[0-128, 128-256, 256-384, 384-512, 512-640, 640-768, 786-896, 896-1024, 1024-1152, 1152-1280]
```


## Signal ##
Since the storage logic, each signal will have the following form indicated by the example with 2 button presses with 2 time slots

```
Button press
^
|      |       |
|      |       |
|      |       |
|      |       |
|      |       |
|      |       |
|      |       |
|      |       |
|      |       |
|      |       |
L______|_______|______________________________________________________________________ >ticks
      128     256     384     512     640     768     896     1024     1152     1280    
```


### Log from program ### 
Accetance threshold of min cost is set of 6. An acccetance lvl of 6 came from experiments

```
00:48.526	ID:1	[DBG : Knock     ] Current login is : 
00:48.528	ID:1	1-1-0-0-0-0-0-0-0-0-0-0-0-
00:48.587	ID:1	[DBG : Knock     ] printing dist matrix:
00:48.591	ID:1	0 	0 	1 	1 	1 	1 	1 	1 	1 	1 	1 	1 	1 	
00:48.595	ID:1	0 	0 	1 	1 	1 	1 	1 	1 	1 	1 	1 	1 	1 	
00:48.600	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.604	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.608	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.612	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.616	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.620	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.624	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.628	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.632	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.636	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.640	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.655	ID:1	[DBG : Knock     ] Calculationg cost matrix was a succes 
00:48.659	ID:1	0 	0 	1 	1 	1 	2 	1 	1 	1 	1 	1 	1 	1 	
00:48.663	ID:1	0 	0 	1 	2 	2 	2 	2 	2 	2 	2 	2 	2 	2 	
00:48.667	ID:1	1 	1 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.671	ID:1	2 	2 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.675	ID:1	3 	3 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.679	ID:1	4 	4 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.683	ID:1	5 	5 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.687	ID:1	6 	6 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.691	ID:1	7 	7 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.696	ID:1	8 	8 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.700	ID:1	9 	9 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.704	ID:1	10 	10 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.708	ID:1	11 	11 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	0 	
00:48.710	ID:1	diagonal prio 0-> 
00:48.711	ID:1	diagonal 0-> 
00:48.712	ID:1	diagonal prio 0-> 
00:48.713	ID:1	diagonal prio 0-> 
00:48.715	ID:1	diagonal prio 0-> 
00:48.716	ID:1	diagonal prio 0-> 
00:48.718	ID:1	diagonal prio 0-> 
00:48.719	ID:1	diagonal prio 0-> 
00:48.720	ID:1	diagonal prio 0-> 
00:48.722	ID:1	diagonal prio 0-> 
00:48.723	ID:1	diagonal prio 0-> 
00:48.724	ID:1	diagonal prio 0-> 
00:48.725	ID:1	min cost : 0 
00:48.728	ID:1	[DBG : Knock     ] Password is correct! 
00:48.731	ID:1	[DBG : Knock     ] Main finish 
```
