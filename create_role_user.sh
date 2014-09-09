#!/usr/bin/env bash

comma=','
newline='\n'
uid=$1
uname=$2
role=$3
read=$4
write=$5
execute=$6

#set -x

echo $uid | egrep "^[0-9]+$" > /dev/null
if [ $? -ne 0 ]; then
echo "error: uid must be a positive number"; 
echo 'usage: sh create_role_user.sh <uid> <uname> <role> <0/1> <0/1> <0/1>'
exit;
fi

if [ $uid -le 5000 ]; then
	echo "uid should be greater than 5000"
	echo 'usage: sh create_role_user.sh <uid> <uname> <role> <0/1> <0/1> <0/1>'
	exit 1;
fi

echo $read | egrep "^[0-1]$" > /dev/null
if [ $? -ne 0 ]; then
echo "error: read must be 0 or 1"; 
echo 'usage: sh create_role_user.sh <uid> <uname> <role> <0/1> <0/1> <0/1>'
exit 1;
fi

echo $write | egrep "^[0-1]$" > /dev/null
if [ $? -ne 0 ]; then
echo "error: write must be 0 or 1"; 
echo 'usage: sh create_role_user.sh <uid> <uname> <role> <0/1> <0/1> <0/1>'
exit 1;
fi

echo $execute | egrep "^[0-1]$" > /dev/null
if [ $? -ne 0 ]; then
echo "error: execute must be 0 or 1"; 
echo 'usage: sh create_role_user.sh <uid> <uname> <role> <0/1> <0/1> <0/1>'
exit 1;
fi

useradd -u $uid $uname || { echo 'create user failed. role could not be added'; echo 'usage: sh create_role_user.sh <uid> <uname> <role> <0/1> <0/1> <0/1>' ; exit 1; }

line=$uid$comma$role$comma$read$comma$write$comma$execute
echo $line >> /usr/src/linux-3.14.17/read_policy/policy_file
