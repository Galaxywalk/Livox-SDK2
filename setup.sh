catkin build
cd /work
mkdir build
cd /work/build
cmake .. && make -j
sudo make install

cd /work
echo source devel/setup.bash >> ~/.bashrc
/bin/bash