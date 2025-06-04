FROM osrf/ros:noetic-desktop-full

SHELL ["/bin/bash", "-c"]

ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# Install Python 3.8 and essentials
RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common \
    sudo \
    lsb-release \
    python3.8 \
    python3.8-dev \
    python3.8-distutils \
    python3-pip \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set Python 3.8 as default
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.8 1 && \
    update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 1 && \
    ln -sf /usr/bin/python3.8 /usr/bin/python && \
    python -m pip install --upgrade pip

# Add ROS and workspace to .bashrc
RUN echo "source /opt/ros/noetic/setup.bash" >> ~/.bashrc && \
    echo "source /root/catkin_ws/devel/setup.bash" >> ~/.bashrc

# Set working directory (will be overridden by volume mount)
WORKDIR /root/catkin_ws
