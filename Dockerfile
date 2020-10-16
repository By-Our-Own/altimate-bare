FROM ubuntu:18.04

ARG GCC_ARM_NAME=gcc-arm-none-eabi-9-2020-q2-update

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    wget \
 && rm -rf /var/lib/apt/lists/*

RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/${GCC_ARM_NAME}-x86_64-linux.tar.bz2 && \
        tar xf ${GCC_ARM_NAME}-x86_64-linux.tar.bz2 && \
        rm -f ${GCC_ARM_NAME}-x86_64-linux.tar.bz2 && \
        mv ${GCC_ARM_NAME} /opt/${GCC_ARM_NAME}

# ENV TOOLCHAIN_PATH=./${GCC_ARM_NAME}
# RUN echo ::set-output name=toolchain_path::"./gcc-arm-none-eabi-9-2020-q2-update/"
RUN ls /opt
RUN ls /opt/${GCC_ARM_NAME}
RUN ls /opt/${GCC_ARM_NAME}/bin

CMD ["/bin/bash"]
