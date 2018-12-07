
To create character device: 
sudo mknod -m 666 /dev/TIMER_DEV c 503 0

p8 & p9 headers: https://github.com/derekmolloy/boneDeviceTree/blob/master/docs/BeagleboneBlackP8HeaderTable.pdf

usage:
./test [-w N] : -w: write [N: message]
./test [-r]  read current timer value
./test [-d N] : Duty cycle change [N: percentage of duty cycle]
./test [-t N]  Tick duration change [N : #ms]