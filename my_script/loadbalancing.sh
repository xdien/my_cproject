#!/bin/sh

# EXT1 = network interface #1 that connect to adsl modem #1
# EXT2 = network interface #2 that connect to adsl modem #2
# INT  = network interface for internal network
# IPEXT1 = ip address for network interface #1
# IPEXT2 = ip address for network interface #2
# GWEXT1 = gateway ip address for network interface #1
# GWEXT2 = gateway ip address for network interface #2

IPT="/usr/sbin/iptables"
IP="/bin/ip"
EXT1="ppp0"
EXT2="ppp1"
INT="eth0"
IPEXT1=`ifconfig $EXT1 | grep inet | awk '{ print $2 }'`
IPEXT2=`ifconfig $EXT2 | grep inet | awk '{ print $2 }'`
GWEXT1=`route -n | grep $EXT1 | awk '{ print $1 }'`
GWEXT2=`route -n | grep $EXT2 | awk '{ print $1 }'`

echo 1 >| /proc/sys/net/ipv4/ip_forward
echo 0 >| /proc/sys/net/ipv4/conf/all/rp_filter

# flush all IPT entries
$IPT -t filter -F
$IPT -t filter -X
$IPT -t nat -F
$IPT -t nat -X
$IPT -t mangle -F
$IPT -t mangle -X
$IPT -t filter -P INPUT ACCEPT
$IPT -t filter -P OUTPUT ACCEPT
$IPT -t filter -P FORWARD ACCEPT

# initialise chains that will do the work and log the packets
$IPT -t mangle -N CONNMARK1
$IPT -t mangle -A CONNMARK1 -j MARK --set-mark 1
$IPT -t mangle -A CONNMARK1 -j CONNMARK --save-mark
$IPT -t mangle -A CONNMARK1 -j LOG --log-prefix 'IPT-mark1: ' --log-level info

$IPT -t mangle -N CONNMARK2
$IPT -t mangle -A CONNMARK2 -j MARK --set-mark 2
$IPT -t mangle -A CONNMARK2 -j CONNMARK --save-mark
$IPT -t mangle -A CONNMARK2 -j LOG --log-prefix 'IPT-mark2: ' --log-level info

$IPT -t mangle -N RESTOREMARK
$IPT -t mangle -A RESTOREMARK -j CONNMARK --restore-mark
$IPT -t mangle -A RESTOREMARK -j LOG --log-prefix 'restore-mark: ' --log-level info

$IPT -t nat -N SNAT1
$IPT -t nat -A SNAT1 -j LOG --log-prefix 'snat-to-192.168.0.103: ' --log-level info
$IPT -t nat -A SNAT1 -j SNAT --to-source $IPEXT1

$IPT -t nat -N SNAT2
$IPT -t nat -A SNAT2 -j LOG --log-prefix 'snat-to-192.168.42.179: ' --log-level info
$IPT -t nat -A SNAT2 -j SNAT --to-source $IPEXT2

# restore the fwmark on packets that belong to an existing connection
$IPT -t mangle -A PREROUTING -i $INT -p tcp \
-m state --state ESTABLISHED,RELATED -j RESTOREMARK

# if the mark is zero if means the packet does not belongs to an existing connection
$IPT -t mangle -A PREROUTING -p tcp -m state --state NEW \
-m statistic --mode nth --every 2 --packet 0 -j CONNMARK1
$IPT -t mangle -A PREROUTING -p tcp -m state --state NEW \
-m statistic --mode nth --every 2 --packet 1 -j CONNMARK2

$IPT -t nat -A POSTROUTING -o $EXT1 -j SNAT1
$IPT -t nat -A POSTROUTING -o $EXT2 -j SNAT2

if ! cat /etc/iproute2/rt_tables | grep -q '^251'
then
echo '251     rt_link1' >> /etc/iproute2/rt_tables
fi
if ! cat /etc/iproute2/rt_tables | grep -q '^252'
then
echo '252     rt_link2' >> /etc/iproute2/rt_tables
fi

$IP route flush table rt_link1 2>/dev/null
$IP route add table rt_link1 default via $GWEXT1 dev $EXT1

$IP route flush table rt_link2 2>/dev/null
$IP route add table rt_link2 default via $GWEXT2 dev $EXT2

$IP rule del from all fwmark 0x1 lookup rt_link1 2>/dev/null
$IP rule del from all fwmark 0x2 lookup rt_link2 2>/dev/null
$IP rule del from all fwmark 0x2 2>/dev/null
$IP rule del from all fwmark 0x1 2>/dev/null
$IP rule add fwmark 1 table rt_link1
$IP rule add fwmark 2 table rt_link2

$IP route flush cache
