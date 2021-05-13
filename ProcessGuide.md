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

#### The result might be:
![start](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/figures/Result%20of%20Start.png)

### 2.2 Construct the map
#### roslaunch [ConstructMap.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/ConstructMap.launch) (_Turtlebot_)
>Run the launch file to construct the map with the lidar sensor

#### roslaunch turtlebot_teleop [keyboard_teleop.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/keyboard_teleop.launch) (_Turtlebot_)
> Run this module to control the movement of the turtlebot2 with keyboard

#### roslaunch turtlebot_rviz_launchers [view_navigation.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/view_navigation.launch) (_Workstation_)
> The visualization result on rviz

#### rosrun map_server map_saver -f /tmp/my_map (_Turtlebot_)
>Save the map in this path /tmp/my_map

#### The result might be:
![start](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/figures/Result%20of%20Constructing%20the%20Map.png)


### 2.3 Mark Detection

#### roslaunch [MarkDetection.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/MarkDetection.launch)  map_file:=/temp/my_map.yaml (_Turtlebot_)
> Run the launch file to detect the ArUco mark with the Kinect camera

#### roslaunch turtlebot_teleop [keyboard_teleop.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/keyboard_teleop.launch) (_Turtlebot_)
> Run this module to control the movement of the turtlebot2 with keyboard

#### rosrun aruco_ros [store_cpp](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/cpp/store.cpp) (_Workstation_)
> Run this module to see the coordinate and the marker Id

#### The result might be:
![start](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/figures/Result%20of%20Mark%20Detection.png)

### 2.4 Navigation
#### roslaunch turtlebot_navigation [amcl_demo.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/amcl_demo.launch)  map_file:=/temp/my_map.yaml (_Turtlebot_)
#### roslaunch turtlebot_rviz_launchers [view_navigation.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/view_navigation.launch) (_Workstation_)
> The visualization route planning on rviz

#### rosrun nav_test_ws [simple_navigation_goals](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/cpp/navigation.cpp) (_Turtlebot_)
> Run the navigation module for different target

#### The result might be:
![start](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/figures/Result%20of%20navigation_rviz.png)
