xhost +local:root

docker run -it --rm \
  --net=host \
  --env="DISPLAY=$DISPLAY" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --volume="./:/work" \
  ros1-livox:latest /work/setup.sh