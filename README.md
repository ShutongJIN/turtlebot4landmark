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

### Path planning --- move_base

### Recognizing the landmark---ArUco Marker

## Examples
### save
```cpp

```

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
