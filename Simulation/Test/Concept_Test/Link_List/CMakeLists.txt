#cmake -G "MinGW Makefiles" ../src
# mkdir build
# cd build
# make
cmake_minimum_required(VERSION 2.6)
project(Simulation)
add_executable(Simulation NB_IoT_eNB_Scheduler.cpp NB_Channel_Config.cpp)

