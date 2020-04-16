#!/bin/sh

#OUTPUT_PIN=(8 2 3 4)


#for i in ${OUTPUT_PIN[@]}; do
#	echo "$i, ${OUTPUT_PIN[$i]}"
#done

gpio_set_out()
{
	echo $1 > export
	echo out > gpio$1/direction
}

gpio_set_in()
{
	echo $1 > export
}

cd /sys/class/gpio

#DO-2 gpio3-14
gpio_set_out 110
ln -s /sys/class/gpio/gpio110/value /dev/do1
#DO-1 gpio3-18
gpio_set_out 114
ln -s /sys/class/gpio/gpio114/value /dev/do2
#PPS-SEL gpio3-21
#gpio_set_out 117
#ln -s /sys/class/gpio/gpio117/value /dev/pps-sel
#WI gpio0-20
gpio_set_out 20
ln -s /sys/class/gpio/gpio20/value /dev/wdi
#WDS gpio3-15
#gpio_set_out 111
#ln -s /sys/class/gpio/gpio111/value /dev/wds
#LED-RUN gpio0-17
gpio_set_out 17
ln -s /sys/class/gpio/gpio17/value /dev/led-run
#LED_MASTER gpio0-7
gpio_set_out 7
ln -s /sys/class/gpio/gpio7/value /dev/led-master

#DI-1 gpio3-16
gpio_set_in 112
ln -s /sys/class/gpio/gpio112/value /dev/di0
#DI-2 gpio3-17
gpio_set_in 113
ln -s /sys/class/gpio/gpio113/value /dev/di1

#init gpio status
echo 1 > /dev/led-master

cd -

echo "gpio set finish..."
