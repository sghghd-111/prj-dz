#! /bin/sh

PARAM_PATH="/opt/param/"
CMS_FLAG_FILE="wddhxws"

if [ ! -f $PARAM_PATH$CMS_FLAG_FILE ];then
	echo "$PARAM_PATH$CMS_FLAG_FILE not exist, touch it..."
	touch $PARAM_PATH$CMS_FLAG_FILE
	sync
fi

