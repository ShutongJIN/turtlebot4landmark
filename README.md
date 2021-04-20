<!--script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=default"></script-->
<!--Note: the above code is used to insert mathematical formula-->
<!--well but seems likes it is useless, in atom it works, but in chrome u probaly have to install a external plugin called MathJax Plugin for Github-->
# **Landmark Detection Based on TurtleBot**
Reminder: There exist some math formulas in this report. If you want to browse it properly, probably you have to install a external plugin called MathJax Plugin for Github for chrome browser.



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

### Path planning --- move_base

### Recognizing the landmark---ArUco Marker

## Examples
### save


### simple_navigation_goals.cpp
In this code, we realized the function of navigating the TurtleBot based on the given instructions, which is the value of the ArUco marker. And the gists of the code is as following:  
* Transforming the relative coordinates into absolute coordinates, which facilites the navigition.
* Move_base is used for path planing where absolute coordinates are used.
* The loop can be terminated through inputing a breaknumer, which is defined in the code.

```c++

#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <cstdio>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  //test-array
  //the first index - value of the ArUco marker
  //the second index - x in the coordinates
  //the third index - y in the coordinates
  int a[6][3]={{0,0,0},{1,1,1},{2,2,2},{3,3,3},{4,-3,-3},{5,0,0}};

  int marker=0;
  int breaknumer=10;//you can change this later
  //test-loop
  //the condition can be changed due to the actual circumstance
  while(marker!=breaknumber){
    printf("Please input the value of the landmark:\n");
    int temp=marker;//temp is used to save the former value
    //because we are using absolute, not relative, coordinates in this project
    scanf("%d",&marker);
    if(marker==breaknmber){
      break;
    }

    ros::init(argc, argv, "simple_navigation_goals");

    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);

    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the move_base action server to come up");
    }

    move_base_msgs::MoveBaseGoal goal;

    goal.target_pose.header.frame_id = "base_link";
    goal.target_pose.header.stamp = ros::Time::now();

    goal.target_pose.pose.position.x = a[marker][1] - a[temp][1];
    goal.target_pose.pose.position.y = a[marker][2] - a[temp][2];
    goal.target_pose.pose.orientation.w = 1.0;

    ROS_INFO("Sending goal");
    ac.sendGoal(goal);

    ac.waitForResult();

    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      printf("Hooray, the base moved %d meter in the direction of x and %d meter in the direction of y\n", a[marker][1],a[marker][2]);
    else
      printf("The base failed to move for some reason\n");
  }
  printf("Voila! This is the end!\n");
  return 0;
}

```
