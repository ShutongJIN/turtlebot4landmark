# Guidance for the Landmark Detection Project
###### Created by Ke GUO and Shutong JIN.  
###### Ps: For running the ConstructMap.launch file, you have to at least connect the base and the lidar with your laptop. For running the MarkDetection,launch, you have to at least connect the base, the camera and the lidar with your laptop.
## 1. Preparation
##### In order to successfully implement this project, the minimum equipment requirements are two computers, one for the workstation and the other is called turtlebot which is connected with the turtlebot2. You also need a router for the communication between those two terminals. For the turtlebot2, it is better to be equipped with Hokuyo lidar and the Kinect camera.

##### Before starting ROS, You need to make sure the workstation and the turtlebot are connected with the same router. And you have to check IP adress. Then edit the _~/.bashrc_ file for the next step. The specific steps can be viewed in this [document](http://faculty.rwu.edu/mstein/verbiage/EMARO%20Mapping%20Report.pdf), which will not be expanded in this guidance.

## 2. Process
### 2.1 Set the connect between two computers
##### PS: Everytime you want to run the file on the turtlebot, you have to connect it on the workstation by using the folloing command in a new terminal.
#### ssh -X turtle@192.168.1.4(_Workstation_)
>Set the connect between the workstation and the turtlebot.  
Here 192.168.1.4 is the IP adress of the turtlebot.

#### roscore (_Turtlebot_)
>Start the ROS system


### 2.2 Construct the map
#### roslaunch [ConstructMap.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/ConstructMap.launch)
>Run the launch file to construct the map with the lidar sensor

#### roslaunch turtlebot_teleop keyboard_teleop.launch
> Run this module to control the movement of the turtlebot2 with keyboard

#### roslaunch turtlebot_rviz_launchers view_navigation.launch
> The visualization result on rviz

#### rosrun map_server map_saver -f /tmp/my_map
>Save the map in this path /tmp/my_map


### 2.3 Mark Detection

#### roslaunch [MarkDetection.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/MarkDetection.launch)  map_file:=/temp/my_map.yaml
> Run the launch file to detect the ArUco mark with the Kinect camera

#### rosrun aruco_ros [store_cpp](need a link)
> Run this module to see the coordinate and the marker Id


### 2.4 Navigation
#### roslaunch turtlebot_rviz_launchers view_navigation.launch
> The visualization route planning on rviz

#### rosrun nav_test_ws [simple_navigation_goals](need a link change the name....)
> Run the navigation module for different target
