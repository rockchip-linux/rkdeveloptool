FROM debian:stable-slim
WORKDIR /rkdeveloptool
COPY . .
RUN apt update && \
  apt install -y g++ make pkg-config libudev-dev libusb-1.0-0-dev dh-autoreconf && \
  ./autogen.sh && \
  ./configure && \
  make

FROM debian:stable-slim
COPY --from=0 /rkdeveloptool/rkdeveloptool /usr/local/bin/
RUN apt update && \
  apt install -y libusb-1.0-0 && \
  rm -rf /var/lib/apt/lists
ENTRYPOINT ["/usr/local/bin/rkdeveloptool"]
