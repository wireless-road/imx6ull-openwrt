#!/bin/bash
set -ex

: ${EXPORT_DIR:="/nfsshare"}
: ${EXPORT_OPTS:="*(rw,fsid=0,insecure,no_root_squash,no_subtree_check,sync)"}

mkdir -p $EXPORT_DIR
echo "$EXPORT_DIR   $EXPORT_OPTS" > /etc/exports


mount -t nfsd nfsd /proc/fs/nfsd
# Fixed nlockmgr port
echo 'fs.nfs.nlm_tcpport=32768' >> /etc/sysctl.conf
echo 'fs.nfs.nlm_udpport=32768' >> /etc/sysctl.conf
sysctl -p > /dev/null

rpcbind -w
rpc.nfsd -N 2 -V 3 -N 4 -N 4.1 8
exportfs -arfv
rpc.statd -p 32765 -o 32766
rpc.mountd -N 2 -V 3 -N 4 -N 4.1 -p 32767 -F
