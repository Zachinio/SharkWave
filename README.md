# SharkWave
Tool to sniff network traffic per process id or package name used by libpcap.so .

# Requirements
 -  rooted device.
 -  libpcap.so must exist on your device - it should be in "system/lib" or "system/lib64", if it doesn't exist at all please open an issue with your device info.

# What SharkWave Solves
There are many tools that can be used in Android to sniff the network traffic like Wireshark and TCPDump, but these tools don't filter the packets by a process id or a package name, therefore Sharkwave is being developed to sniff network of a specific app or any process.

# Current State
For now, the tool sniffs only one network device at the time.
It's possible to execute SharkWave without a --device argument and the tool would find a network device by using the pcap_lookupdev method in libpcap.
Only TCP/UDP packets are filtered now.


# How To Use
To sniff a specific package:
```
SharkWave --device wlan0 --package com.android.chrome
```
To sniff a specific process id:
```
SharkWave --device wlan0 --pid 16535
```
