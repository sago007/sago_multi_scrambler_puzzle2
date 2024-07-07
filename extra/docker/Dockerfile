FROM ubuntu:24.04

# Set timezone to UTC. This prevents apt-get install SOME-PACKAGES from asking for timezone during setup
ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install --no-install-recommends -y build-essential libphysfs-dev libboost-dev cmake libsdl2-dev libsdl2-image-dev \
libsdl2-gfx-dev libsdl2-mixer-dev libsdl2-ttf-dev libboost-program-options-dev libimgui-dev zip gettext

RUN mkdir -p /staging/game

COPY . /staging/game

RUN cd /staging/game && \
cmake . && \
make
