# SIGHANDLERS: C++11 Signal Hanlders code for portable research applications

In order to focus on ground-breaking research, we, the scientists, generally write quick & dirty code. So debugging of such code is essential. SIGHANDLERS allows to "freeze" a failing process on supercomputer, such that a developer can reach out the node, attach debugger and track down the cause.

## Usage

```sh
./sighandlers_sah_test 
You may want to set FREEZE_ON_ERROR environment variable to debug the case
Aborted (core dumped)

FREEZE_ON_ERROR=1 ./sighandlers_sah_test 
Aborted
Process is waiting to attach the debugger on host M17xR4
^C
```

```sh
./sighandlers_sfh_test 
You may want to set FREEZE_ON_ERROR environment variable to debug the case
Segmentation fault (core dumped)

FREEZE_ON_ERROR=1 ./sighandlers_sfh_test 
Segmentation fault
Process is waiting to attach the debugger on host M17xR4
```

## Building

```
mkdir build
cd build
cmake ..
make
```

