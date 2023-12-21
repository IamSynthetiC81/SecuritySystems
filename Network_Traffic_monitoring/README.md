# 4th Assignement

## Compile
In order to compile our code we just use make
```bash
    make
```
## Solution

### Capturing packets
We use libpcaps provided functions such as:
```c
    pcap_open_live
``` 
and
```c
    pcap_open_offline
``` 
To sniff throught the given adapter's traffic and capture packets that we then apply our filters on.

### Packet handler

The captured packets are processed through our packet handler and the protocol, addresses, ports, payload and headers are determined

### Network flows

We determine network flows by looking at the source and destination IPs and ports and categorize all packets that all these match as a network flow

### Retransmissions

The UDP protocol doesn't support reliable transmission so lost packets are not retransmitted
In TCP however such packets are retransmitted and it is possible to determine whether a packet has been retransmitted through it's sequence number
