#cmake -G "MinGW Makefiles" ../src/ModifedSystemModel/SystemModel_0409
# mkdir build
# cd build
# Just make, it will know the change of CMakeLists.txt
#make -j 2, dual core handle compile
#If Add (1)set_Property or (2)option need delete build folder and re-run cmake -G "MinGW Makefiles" ../src
cmake_minimum_required(VERSION 2.6)
project(Simulation)
add_executable(Simulation NB_Packet_Scheduler.cpp XXX.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
# add_executable(123 NB_IoT_eNB_Scheduler.cpp NB_Channel_Config.cpp)
# set_target_properties(123 PROPERTIES LINKER_LANGUAGE CXX)
