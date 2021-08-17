## Author 
Tang Jiujun <jiujun.tang@gmail.com>
https://github.com/tangjiujun/docker-nfs-server

## Run NFS server

```
$ docker-compose up -d
```

## Test on client side

```
$ sudo mount -v -o vers=3 192.168.31.37/nfs /home/al/nfs
$ ls /home/al/nfs
$ umount /home/al/nfs
```

where `192.168.31.37` is IP address of machine running NFS server and `/home/al/nfs` is folder to mount remote folder.
