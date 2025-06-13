FROM buildpack-deps:jammy

RUN apt update && apt install -y \
    perl pkg-config gettext intltool \
    libx11-dev libxext-dev libxi-dev libxt-dev libxft-dev \
    libxinerama-dev libxrandr-dev libxxf86vm-dev \
    libgl-dev libglu1-mesa-dev libglew-dev \
    libgtk-3-dev libgdk-pixbuf2.0-dev \
    libjpeg-dev libxml2-dev libpam0g-dev \
    libsystemd-dev

RUN git clone https://github.com/Zygo/xscreensaver.git

WORKDIR /xscreensaver

RUN ./configure