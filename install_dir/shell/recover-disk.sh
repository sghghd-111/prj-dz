#! /bin/sh

mount_disk()
{
	echo "mount-disk..."
	echo "$# $@"
	ubiattach /dev/ubi_ctrl -m $3
	if [ $? -ne 0 ]; then
		return 1
	fi
	echo "ubimkvol /dev/ubi$2 -N rfs -s 61MiB"
	ubimkvol /dev/ubi$2 -N rfs -s 61MiB
	mount -t ubifs ubi$2:rfs $1

	return 0
}

format_disk()
{
	echo "format /dev/mtd$1"

	ubidetach /dev/ubi_ctrl -m $1
	ubiformat /dev/mtd$1
}

echo "$#, $@"
echo "p1=$1"
echo "p2=$2"

umount $1
format_disk $3
mount_disk $1 $2 $3

#if [ $? -ne 0 ]; then                       
#        format_disk $2
#        mount_disk $1
#        echo "maybe first start, format %1"
#else                                        
#        echo "$1 mount ok"
#fi

