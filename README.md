<script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=default"></script>
<!--Note: the above code is used to insert mathematical formula-->
<!--well but seems likes it is useless, in atom it works-->
# **Landmark Detection Based on TurtleBot**




## Abstract
In this project, a **kinect turtlebot** is used to map the environment, as well as detect **ArUco markers** and save their coordinates in a 2-dimensional array. Then, on user's request, the turtlebot will go to one of the markers and able to travel between them in the requested order.

## Requirements
### Software packages
* **OS:** ROS Kinetic
* **Language:** C++
* **Dependancies:** ArUco, Amcl, Rviz, move_base, ...

### Hardware
* **Components:** LiDAR Hokuyo, Kinect
* **Other evalution and development systems:** TurtleBot Kobuki base

## Installation and setup
Open Source related to TurtleBot 2  
* [TurtleBot_bringup](http://wiki.ros.org/turtlebot_bringup)
* [TurtleBot_navigation](http://wiki.ros.org/turtlebot_navigation)
* [Rviz](http://wiki.ros.org/rviz)
* [LiDAR Hokuyo](https://blog.csdn.net/Buer_zhu/article/details/80945830)
* [Aruco](http://wiki.ros.org/aruco)
* [usb_cam](http://wiki.ros.org/usb_cam)
* [A small tool for generating ArUco markers](https://tn1ck.github.io/aruco-print/)   
...

## Architecture
![](./architecture.png)

## Principle
### Constructing the map --- gmapping
When constructing the map, we use one _.launch_ file called _hokuyo_gmapping_demo.launch_, which mainly calls this ROS package
_[slam_gmapping](http://wiki.ros.org/gmapping)_. This package provides laser-based SLAM (Simultaneous Localization and Mapping), as a ROS node called _slam_gmapping_. By using _slam_gmapping_, we can create a 2-D occupancy grid map (like a building floorplan) from laser and pose data collected by a mobile robot.

Actually, there are several different packages on ROS to achieve the SLAM algorithm, such as _HectorSLAM_, _LagoSLAM_, _KartoSLAM_,etc. But for _GmappingSLAM_, it performs better in a smaller environment and does not need a laser scanner with high update frequency. The advantages and characteristics of _GmappingSLAM_ are consistent with the actual situation of our project. That is the reason why we chose this package.

_GmappingSLAM_ uses an algorithm based on RBPF (Rao-Blackwellized particle filter) to solve the SLAM problem. A complete SLAM problem is to estimate the robot pose and map at the same time with given sensor data. However, if you need to get an accurate pose, you need to match it with the map. If you need to get a good map, you need an accurate pose to do it. Obviously, this is a contradictory problem like _Chicken-and-egg problem_. Explain it in a mathematical way, the SLAM problem can be expressed as the following form:
$$p(x_{1:t},m|u_{1:t},z_{1:t})$$
where $z_{1:t}$ is a series of sensor measurement data from bringing up the robot to $t$, $u_{1:t}$ is a series of control data (in ROS typically considered as odom) , and the map information $m$ and the robot trajectory state $x_{1:t}$ are what we want to estimate.

According to probability theory, the above formula can be simplified to:
$$p(x_{1:t}|u_{1:t},z_{1:t})p(m|x_{1:t},z_{1:t})$$
In this way, the SLAM problem is reduced to two problems. Among them, the map construction of the known robot pose is a simple problem, so the estimation of the robot pose is a key issue.

In _GmappingSLAM_, it uses particle filtering to estimate the pose of the robot and build a map for each particle. Therefore, each particle contains the robot's trajectory and the corresponding environment map. Here we use Bayesian formula to simplify the calculation of pose. (The specific derivation is quite complicated, here I only explain the results)
$$p(x_{1:t}|u_{1:t},z_{1:t}) = {\eta}p(z_{t}|x_{t})p(x_{t}|x_{t-1},u_{t})p(x_{1:t-1}|u_{1:t-1},z_{1:t-1})$$
where using $p(x_{1:t-1}|u_{1:t-1},z_{1:t-1})$ to represent the robot trajectory at the previous moment, $p(x_{t}|x_{t-1},u_{t})$ is the kinematic model for propagation so that we can obtain the predicted trajectory of each particle. For each particle after propagation, the observation model $p(z_{t}|x_{t})$ is used for weight calculation and normalization, so that the robot trajectory at that moment is obtained.

But when the detected environment is quite large, we need tons of particles to estimate the pose, which is unreasonable in actual operation. In _GmappingSLAM_, it uses the maximum likelihood estimation to optimize this problem. According to the predicted distribution of the particle's pose and the matching degree with the map, the optimal pose parameter of the particle is found through scanning matching. Then this parameter is used directly as the position of the new particle posture. Through this way, the quality of sampling is highly improved.
