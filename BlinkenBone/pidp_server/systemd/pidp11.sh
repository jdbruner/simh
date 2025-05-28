#! /bin/bash

# pidp11 server script
#
# This script should be run as an ordinary user (not root)

# Create temporary file for SIMH commands
bootscript=$(mktemp -p /tmp pidp11.XXXXXX)

# make sure the panel server is ready
until rpcinfo -T tcp localhost 99 1 > /dev/null 2>&1
do
	sleep 1
done

while
	# select system using low 12 bits of SR switches
	eval declare -A selections=($(/opt/pidp11/systems/get_selections.sh))
	csw=$(/opt/pidp11/getcsw -o4 -0 -n12)
	sel=${selections[${csw:-"0000"}]:-"idled"}
	echo "*** booting $sel ***"
	# create a bootscript for simh in the /run ramdisk:
	eval cat > $bootscript <<-EOF
		cd /opt/pidp11/systems/$sel
		do boot.ini
	EOF
	echo "*** Start client ***"
	/opt/pidp11/client11 -q $bootscript
do
	:
done

rm $bootscript
