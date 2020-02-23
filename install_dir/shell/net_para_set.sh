#! /bin/sh

PARA_DIR="/opt/param/"

#NET_SETS=(eth0, eth1, eth2, eth3)

NET_IDX=1

set_addr()
{
	ifconfig $1 $2 netmask $3
	echo "$#"
	echo "$@"

}

set_mac()
{
	ifconfig $1 down
	ifconfig $1 hw ether $2
	ifconfig $1 up
}

set_net_param()
{
	mask="255.255.255.0"


	mac_path="$PARA_DIR$1-mac"
	if [ -f $mac_path ]; then
		mac_addr=`cat $mac_path`
		set_mac $1 $mac_addr
	fi

	ipaddr_path="$PARA_DIR$1-ipaddr"
#	ipaddr_path=`cat $PARA_DIR$1-ipaddr`
	echo $ipaddr_path
	if [ -f $ipaddr_path ]; then
		ipaddr=`cat $ipaddr_path`
	else
		ipaddr="192.168.$NET_IDX.112"
		echo "not exist, default $ipaddr"
		NET_IDX=`expr $NET_IDX + 1`
#		return 0
	fi

	mask_path="$PARA_DIR$1-mask"
	if [ -f $mask_path ]; then
		mask=`cat $mask_path`
	else
		echo "$mask_path not exist"
	fi

	set_addr $1 $ipaddr $mask
}

echo $PARA_DIR

set_net_param eth0
set_net_param eth1
set_net_param eth2
set_net_param eth3

