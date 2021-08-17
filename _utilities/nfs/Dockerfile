FROM alpine:3.6

MAINTAINER Tang Jiujun <jiujun.tang@gmail.com>
# https://github.com/tangjiujun/docker-nfs-server

RUN set -ex && { \
        echo 'http://mirrors.aliyun.com/alpine/v3.6/main'; \
        echo 'http://mirrors.aliyun.com/alpine/v3.6/community'; \
    } > /etc/apk/repositories \
    && apk update && apk add bash nfs-utils && rm -rf /var/cache/apk/*

EXPOSE 111 111/udp 2049 2049/udp \
    32765 32765/udp 32766 32766/udp 32767 32767/udp 32768 32768/udp

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
