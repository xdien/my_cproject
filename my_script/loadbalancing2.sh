#!/bin/bash
#
# bal_local        Load-balance internet connection over two local links
#
# Version:         1.0.0 - Fri, Sep 26, 2008
#
# Author:          Niels Horn
#

# Set devices:
DEV1=enp0s29u1u4   # default eth0
DEV2=wlp5s0    # default ppp0

# Get IP addresses of our devices:
#ip1=`ifconfig $DEV1 | grep inet | awk '{ print $2 }' | awk -F: '{ print $2 }'`
#ip2=`ifconfig $DEV2 | grep inet | awk '{ print $2 }' | awk -F: '{ print $2 }'`
ip1=192.168.42.236
ip2=192.168.0.103

# Get default gateway for our devices:
#gw1=`route -n | grep $DEV1 | grep '^0.0.0.0' | awk '{ print $2 }'`
#gw2=`route -n | grep $DEV2 | grep '^0.0.0.0' | awk '{ print $2 }'`
gw1=192.168.42.129
gw2=192.168.0.1

echo "$DEV1: IP=$ip1 GW=$gw1"
echo "$DEV2: IP=$ip2 GW=$gw2"

### Definition of routes ###

# Check if tables exists, if not -> create them:
if [ -z "`cat /etc/iproute2/rt_tables | grep '^251'`" ] ; then
   echo "251    rt_dev1" >> /etc/iproute2/rt_tables
fi
if [ -z "`cat /etc/iproute2/rt_tables | grep '^252'`" ] ; then
   echo "252    rt_dev2" >> /etc/iproute2/rt_tables
fi

# Define routing tables:
ip route add default via $gw1 table rt_dev1
ip route add default via $gw2 table rt_dev2

# Create rules:
ip rule add from $ip1 table rt_dev1
ip rule add from $ip2 table rt_dev2

# If we already have a 'nexthop' route, delete it:
if [ ! -z "`ip route show table main | grep 'nexthop'`" ] ; then
   ip route del default scope global
fi

# Balance links based on routes:
ip route add default scope global nexthop via $gw1 dev $DEV1 weight 1 \
   nexthop via $gw2 dev $DEV2 weight 1

# Flush cache table:
ip route flush cache
