cmake_minimum_required(VERSION 2.8.3)
project(led)

add_definitions(-Wall -std=c++11)
find_package(catkin REQUIRED COMPONENTS
    roscpp rospy sensor_msgs image_transport cv_bridge
    )
find_package(OpenCV REQUIRED)

catkin_package(
)

include_directories(
    ${catkin_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    )

add_executable(led_node src/led_node.cpp)
target_link_libraries(led_node
    ${catkin_LIBRARIES}
    ${OpenCV_LIBRARIES}
    )
