#! /bin/bash
# Install PiDP-11/70
# This assumes it has been fully built with make.sh

PIDP11_USER=pidp11
PIDP11_GROUP=${PIDP11_USER}
PIDP11_HOME=/home/${PIDP11_USER}
PIDP11_BIN=${PIDP11_HOME}/bin
PIDP11_OPT=/opt/pidp11
SIMH_BIN=../BIN
PIDP_SERVER_DIR=pidp_server
PIDP_SYSTEMS_DIR=${PIDP_SERVER_DIR}/systems
PIDP_SYSTEMD_DIR=${PIDP_SERVER_DIR}/systemd
SYSTEMD_LOCAL_SYSTEM=/usr/local/lib/systemd/system

if [ `id -u` -ne 0 ]; then
    echo "This script must be run as root"
    exit 1
fi

if [ ! -e /usr/bin/raspi-config ]; then
    echo "PiDP-11/70 can only be installed on a Raspberry Pi"
    exit 1
fi

# Create user "pidp11" if it does not already exist
# Create ~/bin and populate with the pdp.sh script
# exec pdp.sh as the final action in .profile
#
if ! id ${PIDP11_USER} > /dev/null 2>&1; then
    useradd -m ${PIDP11_USER}
elif [ ! -d ${PIDP11_HOME} ]; then
    echo "User '${PIDP11_USER}' exists but home directory '${PIDP11_HOME}' does not"
    exit 1
fi

if [ ! -e ${PIDP11_BIN} ]; then
    mkdir -m 755 ${PIDP11_BIN} && chown ${PIDP11_USER}:${PIDP11_GROUP} ${PIDP11_BIN}
fi

cp ${PIDP_SYSTEMD_DIR}/pdp.sh ${PIDP11_BIN}
chown ${PIDP11_USER}:${PIDP11_GROUP} ${PIDP11_BIN}/pdp.sh

if ! grep -q pdp.sh ${PIDP11_HOME}/.profile ; then
    echo 'exec $HOME/bin/pdp.sh' >> ${PIDP11_HOME}/.profile
fi

# Create /opt/pidp11 if it does not exist
# Copy in the executables and script
#
mkdir -p -m 755 ${PIDP11_OPT}
cp -m 755 ${PIDP_SERVER_SRC}/pidp11.sh ${PIDP11_OPT}
cp -m 755 ${SIMH_BIN}/pdp11 ${PIDP11_OPT}/client11
cp -m 755 ${SIMH_BIN}/pidp1170_blinkenlightd ${PIDP11_OPT}/server11
cp -m 755 ${SIMH_BIN}/getcsw ${PIDP11_OPT}/getcsw

# If /opt/pidp11/system already exists, leave it alone
# Otherwise, create an initial version (with idled)
#
if [ ! -d ${PIDP_SYSTEMS_DIR} ]; then
    cp -r ${PIDP_SYSTEMS_DIR} ${PIDP11_OPT}
    chown -R ${PIDP11_USER}:${PIDP11_GROUP} ${PIDP11_OPT}
fi

# Install the systemd service files and start the services
#
mkdir -p -m 755 ${SYSTEMD_LOCAL_SYSTEM}
cp ${PIDP_SYSTEMD_DIR}/pidp11.service ${SYSTEMD_LOCAL_SYSTEM}
cp ${PIDP_SYSTEMD_DIR}/pidp11panel.service ${SYSTEMD_LOCAL_SYSTEM}

systemctl enable rpcbind
systemctl enable pidp11panel
systemctl enable pidp11
systemctl start pidp11