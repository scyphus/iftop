
# iftop: Interface Traffic Monitoring Tool

## Supported operating systems
* Linux (checking detailed version info)
* Mac OS X (checking detailed version info)

## Required libraries
* ncurses

## How to install
    $ ./configure && make
    # make install

## Command
    $ iftop <interface name> [<additional interface name> ...]

## Example output
    $ iftop en0 lo0
    Interface statistics
    Fri Jan 24 17:54:23 2014
    
                           pkts/s                    kbps
                       TX          RX            TX          RX
    en0     :       87.91       90.90         46.89      457.05
    lo0     :        0.00        0.00          0.00        0.00
