FROM debian:stretch-slim

MAINTAINER danrue drue@therub.org
# https://github.com/danrue/docker-tftpd-hpa

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        tftpd-hpa && \
    rm -rf /var/lib/apt/lists/*

CMD echo -n "Starting " && in.tftpd --version && in.tftpd -L --user tftp -a 0.0.0.0:69 -s -B1468 -v /srv/tftp
