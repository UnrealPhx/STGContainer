FROM ubuntu:16.04

RUN apt-get update && apt-get install -y --no-install-recommends xdg-user-dirs
RUN useradd --create-home --home /home/ue4 --shell /bin/bash --uid 1000 ue4

USER ue4
WORKDIR /home/ue4

COPY --chown=ue4:ue4 ./LinuxServer ./LinuxServer

VOLUME ["/home/ue4/LinuxServer/STGContainer/Saved/Config"]
VOLUME ["/home/ue4/LinuxServer/STGContainer/Saved/Logs"]

ENTRYPOINT ["/home/ue4/LinuxServer/STGContainerServer.sh"]

CMD ["Default", "-log"]

EXPOSE 7777/udp 15000/udp 7787/udp