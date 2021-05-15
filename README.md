<!--script type="text/javascript" src="http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=default"></script-->
<!--Note: the above code is used to insert mathematical formula-->
<!--well but seems likes it is useless, in atom it works, but in chrome u probaly have to install a external plugin called MathJax Plugin for Github-->
# **Landmark Detection Based on TurtleBot**
<font size=1>_Reminder: There exist some math formulas in this report. If you want to browse it properly, a external plugin called MathJax Plugin for Github for chrome browser is recommended. **Or you could check the [LaTex](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/SLAM_and_Landmark_Detection_Based_on_Turtlebot2.pdf) file in this repository**_  
</font>


## Abstract
In this project, a **Turtlebot2 with Kinect camera and Hokuyo lidar** is used to map the environment, as well as detect ArUco markers and save their coordinates with their mark ID in a 2-dimensional array. Then, on user's request, the TurtleBot will go to one of the markers and is able to travel between them in the requested order. Based on the framework of ROS, the map construction is found out via the gmapping\_hokuyo\_demo package, which is a integrated package in ROS with a more suitable design to this project based on the Rao-Blackwellized particle filter algorithm. After obtaining the coordinates of the Turtlebot and pose returned from cv::aruco::estimatePoseSingleMarkers, the specific location of the ArUco marker, together with its ID, are save in a 2-dimensional array, on which the future instructions are based. Finally, the movement between different target marker points is accomplished by the ROS navigation framework with the core principle, adaptive Monte Carlo localization.

## Requirements
### Software packages
* **OS:** ROS Kinetic
* **Language:** C++
* **Dependancies:** ArUco, Gmapping, Amcl, Rviz, move_base, ...

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
<div align=center>
<img src=./figures/architecture.png style="zoom:80%;"/>
</div>

## Principle
### Constructing the map --- Gmapping
When constructing the map, we use one _.launch_ file called _hokuyo_gmapping_demo.launch_, which mainly calls this ROS package
_[slam_gmapping](http://wiki.ros.org/gmapping)_. This package provides laser-based SLAM (Simultaneous Localization and Mapping), as a ROS node called _slam_gmapping_. By using _slam_gmapping_, we can create a 2-D occupancy grid map (like a building floorplan) from laser and pose data collected by a mobile robot.


_GmappingSLAM_ uses an algorithm based on RBPF (Rao-Blackwellized particle filter) to solve the SLAM problem. The SLAM problem can be expressed  in a mathematical way as the following form:
$$p(x_{1:t},m|u_{1:t},z_{1:t})$$
where $z_{1:t}$ is a series of sensor measurement data from bringing up the robot to $t$, $u_{1:t}$ is a series of control data (in ROS typically considered as odom) , and the map information $m$ and the robot trajectory state $x_{1:t}$ are what we want to estimate.

According to probability theory, the above formula can be simplified to:
$$p(x_{1:t}|u_{1:t},z_{1:t})p(m|x_{1:t},z_{1:t})$$
In this way, the SLAM problem is reduced to two problems. Among them, the map construction of the known robot pose is a simple problem, so the estimation of the robot pose is a key issue.

In _GmappingSLAM_, it uses particle filtering to estimate the pose of the robot and build a map for each particle. Therefore, each particle contains the robot's trajectory and the corresponding environment map and uses Bayesian formula to simplify the calculation of pose. (The specific derivation is quite complicated, here I only explain the results)
$$p(x_{1:t}|u_{1:t},z_{1:t}) = {\eta}p(z_{t}|x_{t})p(x_{t}|x_{t-1},u_{t})p(x_{1:t-1}|u_{1:t-1},z_{1:t-1})$$
where using $p(x_{1:t-1}|u_{1:t-1},z_{1:t-1})$ to represent the robot trajectory at the previous moment, $p(x_{t}|x_{t-1},u_{t})$ is the kinematic model for propagation so that we can obtain the predicted trajectory of each particle. For each particle after propagation, the observation model $p(z_{t}|x_{t})$ is used for weight calculation and normalization, so that the robot trajectory at that moment is obtained.

But when the detected environment is quite large, we need tons of particles to estimate the pose, which is unreasonable in actual operation. In _GmappingSLAM_, it uses the maximum likelihood estimation to optimize this problem. According to the predicted distribution of the particle's pose and the matching degree with the map, the optimal pose parameter of the particle is found through scanning matching. Then this parameter is used directly as the position of the new particle posture. Through this way, the quality of sampling is highly improved.

### Landmark Detection
#### Aruco marker
An ArUco marker is a synthetic square marker composed by a wide black border andan inner binary matrix which determines its identifier (id).The black border facilitatesits fast detection in the image and the binary codification allows its identification and theapplication of error detection and correction techniques.[6]The marker size determines thesize of the the internal matrix. For instance a marker size of5x5 is compose by 25 bits,and we use markers of this size as the landmark to be detected.Some examples of ArUcomarkers are shown below:
<div align=center>
<img src=./figures/ArUcoMarker.png style="zoom:30%;"/>
</div>
And these are the reasons why we choose this binary square fiducial marker:

* Easily detected
* Relatively robust
* Less embedded bits
* Possible for extrapolating the orientation

#### Marker Detection
The main goal of this part is to recognize the ArUco marker andreturns its id. This process is comprise of two parts:
1. Detection of marker candidates. In this step the image is analyzed in order to findsquare shapes that are candidates to be markers.
2. After the candidate detection, it is necessary to determine if they are actually markersby analyzing their inner codification. This step starts by extracting the marker bits ofeach marker.The ArUco marker could be recognized on the ASUS laptop, through the kinect cameralocated on TurtleBot, as shown below:

<div align=center>
<img src=./figures/MarkerDetection.png style="zoom:30%;"/>
</div>

#### Pose Estimation
We know that there exists a distance between the TurtleBot and the ArUco marker, as shown below:
<div align=center>
<img src=./figures/PoseEstimation.png style="zoom:40%;"/>
</div>
So our problem is that the coordinates system for the TurleBot is the world coordinate system origin, whose original point is O<sub>w</sub>. And the information captured by the camera is in the camera system, therefore, a transformation between world coordinate W and camera coordinate C should be developed.


The basic idea of this transformation is first rotate the coordinates to the same angls.
First of all, by using CV::aruco::estimatePoseSingleMarkers() function,  two very important data revc(rotation vector) and tevc(translation vector) are obtained, respectively. And from these two data, we can get the coordinates of the camera in the world coordinate system. This is where knowledge of SolvePNP is required. The R and T solved by SolvePNP are the rotation and translation required to transform points in the world coordinate system to the camera coordinate system:
* Rvec - The output rotation vector.
* Tvec - the translation vector of the output.

##### Rotation
The first thing need to be done is turn the rotation vector into a rotation matrix using a Rodrigues' rotation.
```c++
    double rm[9];
    RoteM = cv::Mat(3, 3, CV_64FC1, rm);
    Rodrigues(rvec, RoteM);
    double r11 = RoteM.ptr<double>(0)[0];
    double r12 = RoteM.ptr<double>(0)[1];
    double r13 = RoteM.ptr<double>(0)[2];
    double r21 = RoteM.ptr<double>(1)[0];
    double r22 = RoteM.ptr<double>(1)[1];
    double r23 = RoteM.ptr<double>(1)[2];
    double r31 = RoteM.ptr<double>(2)[0];
    double r32 = RoteM.ptr<double>(2)[1];
    double r33 = RoteM.ptr<double>(2)[2];
```
The next step is to calculate the rotation Angle of each axis according to the rotation matrix.
```cpp
    double thetaz = atan2(r21, r11) / CV_PI * 180;
    double thetay = atan2(-1 * r31, sqrt(r32 * r32 + r33 * r33)) / CV_PI * 180;
    double thetax = atan2(r32, r33) / CV_PI * 180;
```
In this case, thetax, thetay, and thetaz are the rotation angles about x, y, and z respectively.
##### Translation
Here is a core function that we use to calculate the value on each axis.
```cpp
void CodeRotateByX(double y, double z, double thetax, double &outy, double &outz)
{
    double y1 = y;
    double z1 = z;
    double rx = thetax * CV_PI / 180;
    outy = cos(rx) * y1 - sin(rx) * z1;
    outz = cos(rx) * z1 + sin(rx) * y1;
}
```
This becomes the camera's coordinates in the world coordinate system, but one more calculation is required to translate it in the opposite direction:
```cpp
    x = x * -1;
    y = y * -1;
    z = z * -1 - 0.1;
```
Another thing need to be noticed is that z has to minus 0.1 due to the height to the kinect camera At this point, the obtained (x,y,z) are the coordinates of the camera in the world coordinate system with the two-dimensional code as the origin.
#### DataSet Construction

The pose of ArUco marker and location of TurtleBot are output as follow.  
Pose:
<div align=center>
<img src=./figures/camera.png style="zoom:40%;"/>
</div>  
Location:
<div align=center>
<img src=./figures/turtlebot.png style="zoom:40%;"/>
</div>


After successfully received those data above, we save it in a two-dimension array, containing three elements:
* the first index - id of the ArUco marker
* the second index - x in the world coordinates
* the third index - y in the world coordinates

Based on these, we now have the essential data required by navigation.

### Navigation
#### Navigation --- AMCL

In ROS, the navigation is implemented via _amcl\_demo.launch_ file. There are two issues relating to navigation. The first is the global navigation, navigating and locating; the second is local navigation, avoiding obstacles. The specific process of global navigation can be divided into two parts, localizing and moving. AMCL(Adaptive Monte Carlo Localization) is used for localizing and _move\_base_ is used for the movement. The following shows the framework of _move\_base_. It can be seen that the movement plan is also based on localizing. The local navigation is determined by the boundaries of the established map.

<div align=center>
<img src=./figures/move_base.png style="zoom:40%;"/>
</div>


Adaptive Monte Carlo Localization uses a probabilistic localization technique and particle filters to track the pose of the robot against a map loaded from the map server.
AMCL is an improved algorithm of Monte Carlo Localization, which can be simply stated as five steps: randomly generate a bunch of particles, predict next state of the particles, update the weights, resample and compute estimate.

AMCL adds the short-term and long-term weight records to reduce the error. The long-term and short-term weights are maintained to obtain the judgment of whether there is an error resampling. After calculating the value, get the ratio of fast and slow to determine whether to add particles randomly to facilitate subsequent repositioning. The precise pseudocode of AMCL is shown in algorithm.

**Algorithm**
```cpp
$\chi_{t-1}$: The confidence\\
static $\omega_{slow}, \omega_{fast}$: Weights\\
$\bar{\chi}_{t} = \chi_{t} = \emptyset$
\FOR{$m=1  to  M$}
\STATE$x^{[m]}_{t} = $\textbf{sample\_motion\_model}$(u_{t},x^{[m]}_{t-1})$\\
\STATE$\omega^{[m]}_{t} = $\textbf{measurement\_model}$(z_{t},x^{[m]}_{t},m)$\\
\STATE$\bar{\chi}_{t} = \bar{\chi}_{t} + <x^{[m]}_{t},\omega^{[m]}_{t}>$
\STATE$\omega_{avg} = \omega_{avg} + \frac{1}{M}\omega^{[m]}_{t}$
\ENDFOR
\STATE$\omega_{slow} = \omega_{slow} + \alpha_{slow}(\omega_{avg} - \omega_{slow})$\\
\STATE$\omega_{fast} = \omega_{fast} + \alpha_{fast}(\omega_{avg} - \omega_{fast})$\\
\FOR{$m=1 to M$}
\IF{probability $max\{0.0,1 - \frac{\omega_{fast}}{\omega_{slow}}\}$}\\
\STATE add random pose to $\chi_{t}$\\
\ELSE
\STATE draw $i \in \{1,...,N\}$ with probability $\propto \omega^{[i]}_{t}$\\
\STATE add $x^{[m]}_{t}$ to $\chi_{t}$\\
\ENDIF
\ENDFOR
\RETURN $\chi_{t}$\\
```


## Process
<font size=1>_Reminder: For running the ConstructMap.launch file, you have to at least connect the base and the lidar with your laptop. For running the MarkDetection,launch, you have to at least connect the base, the camera and the lidar with your laptop._</font>

## 1. Preparation
##### In order to successfully implement this project, the minimum equipment requirements are two computers, one for the workstation and the other is called turtlebot which is connected with the turtlebot2. You also need a router for the communication between those two terminals. For the turtlebot2, it is better to be equipped with Hokuyo lidar and the Kinect camera.

##### Before starting ROS, You need to make sure the workstation and the turtlebot are connected with the same router. And you have to check IP adress. Then edit the _~/.bashrc_ file for the next step. The specific steps can be viewed in this [document](http://faculty.rwu.edu/mstein/verbiage/EMARO%20Mapping%20Report.pdf), which will not be expanded in this guidance.

## 2. Implementation
### 2.1 Set the connect between two computers
<font size=1>_Reminder:  Everytime you want to run the file on the turtlebot, you have to connect it on the workstation by using the folloing command in a new terminal._</font>

#### ssh -X turtle@192.168.1.4(_Workstation_)
>Set the connect between the workstation and the turtlebot.  
Here 192.168.1.4 is the IP adress of the turtlebot.

#### roscore (_Turtlebot_)
>Start the ROS system

#### The result might be:
<div align=center>
<img src=./figures/Result of Start style="zoom100%;"/>
</div>

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
<div align=center>
<img src=./figures/Result of Constructing the Map style="zoom100%;"/>
</div>


### 2.3 Mark Detection

#### roslaunch [MarkDetection.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/MarkDetection.launch)  map_file:=/temp/my_map.yaml (_Turtlebot_)
> Run the launch file to detect the ArUco mark with the Kinect camera

#### roslaunch turtlebot_teleop [keyboard_teleop.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/keyboard_teleop.launch) (_Turtlebot_)
> Run this module to control the movement of the turtlebot2 with keyboard

#### rosrun aruco_ros [store_cpp](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/cpp/store.cpp) (_Workstation_)
> Run this module to see the coordinate and the marker Id

#### The result might be:
<div align=center>
<img src=./figures/Result of Mark Detection style="zoom100%;"/>
</div>

### 2.4 Navigation
#### roslaunch turtlebot_navigation [amcl_demo.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/amcl_demo.launch)  map_file:=/temp/my_map.yaml (_Turtlebot_)
#### roslaunch turtlebot_rviz_launchers [view_navigation.launch](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/launch%20files/view_navigation.launch) (_Workstation_)
> The visualization route planning on rviz

#### rosrun nav_test_ws [simple_navigation_goals](https://github.com/ShutongJIN/turtlebot4landmark/blob/master/cpp/navigation.cpp) (_Turtlebot_)
> Run the navigation module for different target

#### The result might be:
<div align=center>
<img src=./figures/Result of navigation_rviz style="zoom100%;"/>
</div>


## Examples
### Save the coordinate
```c++
#include <iostream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <aruco_ros/aruco_ros_utils.h>
#include <aruco_msgs/MarkerArray.h>
#include <tf/transform_listener.h>
#include <std_msgs/UInt32MultiArray.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>

void markIdCallback(const std_msgs::UInt32MultiArray msg)
{
    ROS_INFO("I got mark id:[%d]",msg.data[0]);
    std::cout<<" markIdCallback run"<<std::endl;
}

float IdStore(const std_msgs::UInt32MultiArray msg)
{
    float id = static_cast<float >(msg.data[0]);
    return id;
}

void PoseCallback(const geometry_msgs::PoseWithCovarianceStamped location)
{
    ROS_INFO("I got the coordinate:[%f][%f]", location.pose.pose.position.x, location.pose.pose.position.y);
    std::cout<<" markIdCallback run"<<std::endl;
}

int Xstore(const geometry_msgs::PoseWithCovarianceStamped location)
{
    float x = location.pose.pose.position.x;
    return x;
}

int Ystore(const geometry_msgs::PoseWithCovarianceStamped location)
{
    float y = location.pose.pose.position.y;
    return y;
}


int main(int argc, char **argv)
{
    int store = 100;

    while (store != 10) {
        ros::init(argc, argv, "store");
        ros::NodeHandle nh;

        printf("Do you want to store this coordinate?(If u want to store, please input 1):\n");
        scanf("%d", &store);

        if (store == 1) {
            ros::Subscriber id = nh.subscribe("/test1/markers_test_id", 1, markIdCallback);


            std::cout << " for store marker id" << std::endl;

            ros::Subscriber pose = nh.subscribe("/amcl_pose", 1, PoseCallback);
            std::cout << " for store the location" << std::endl;

            //ros::spin();


            ros::Rate loop_rate(1);
            while (ros::ok())
            {
                /*...TODO...*/

                ros::spinOnce();
                loop_rate.sleep();
            }


            //ros::Subscriber a = nh.subscribe("/test1/markers_test_id",10, IdStore);
            //printf("%d", &a);


        } else {
            printf("please input the valid instruction(input 10 for quit)");
            scanf("%d", &store);
            continue;
        }


    }
    return 0;
}

```

### simple_navigation_goals.cpp
And there is a video for this part: [Check this](https://youtu.be/TdmjeqUmM-0).

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
### coordinates_transformation
```cpp
include <iostream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>
​
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <aruco_ros/aruco_ros_utils.h>
#include <aruco_msgs/MarkerArray.h>
#include <tf/transform_listener.h>
#include <std_msgs/UInt32MultiArray.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
​
void markIdCallback(const std_msgs::UInt32MultiArray msg)
{
    ROS_INFO("I got mark id:[%d]",msg.data[0]);
    std::cout<<" markIdCallback run"<<std::endl;
}
​
float IdStore(const std_msgs::UInt32MultiArray msg)
{
    float id = static_cast<float >(msg.data[0]);
    return id;
}
​
void PoseCallback(const geometry_msgs::PoseWithCovarianceStamped location)
{
    ROS_INFO("I got the coordinate:[%f][%f]", location.pose.pose.position.x, location.pose.pose.position.y);
    std::cout<<" markIdCallback run"<<std::endl;
}
​
int Xstore(const geometry_msgs::PoseWithCovarianceStamped location)
{
    float x = location.pose.pose.position.x;
    return x;
}
​
int Ystore(const geometry_msgs::PoseWithCovarianceStamped location)
{
    float y = location.pose.pose.position.y;
    return y;
}
​
​
int main(int argc, char **argv)
{
    int store = 100;
​
    while (store != 10) {
        ros::init(argc, argv, "store");
        ros::NodeHandle nh;
​
        printf("Do you want to store this coordinate?(If u want to store, please input 1):\n");
        scanf("%d", &store);

        ​//DO while loop NEED A COUNTER?

        //*************TEST - my part STARTS here.***********
        //insert input video stream, the axis is also drawn on this stream
        cv::VideoCapture inputVideo;
        inputVideo.open(0);
        cv::Mat cameraMatrix, distCoeffs;//These parameters are obtained from camera calibration
        readCameraParameters(cameraMatrix, distCoeffs);
        // the Dictionary object is created by choosing one of the predefined dictionaries in the aruco module. Concretely, this dictionary is composed of 50 markers and a marker size of 5x5 bits (DICT_5X5_50)
        cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_50);

        while (inputVideo.grab()) {
            cv::Mat image, imageCopy;
            inputVideo.retrieve(image);//Decodes and returns the grabbed video frame
            image.copyTo(imageCopy);
            std::vector<int> ids;
            std::vector<std::vector<cv::Point2f>> corners;
            cv::aruco::detectMarkers(image, dictionary, corners, ids);
            // if at least one marker detected
            if (ids.size() > 0) {
                cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
                std::vector<cv::Vec3d> rvecs, tvecs;
                cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
                //for cv::aruco::estimatePoseSingleMarkers
                //input:corners, 0.05, cameraMatrix, distCoeffs
                //output: rvecs(rotation vectors), tvecs(translation vectors)
                double rm[9];
                RoteM = cv::Mat(3, 3, CV_64FC1, rm);//make sure every pixel has is a 64 bit float
                cv::Rodrigues(rvec, RoteM); //obtain the Rodrigues matrix, used for rotating a vector in space, given an axis and angle of rotation
                //separate each element, which facilitate the calculation of Euler angles
                double r11 = RoteM.ptr<double>(0)[0];
                double r12 = RoteM.ptr<double>(0)[1];
                double r13 = RoteM.ptr<double>(0)[2];
                double r21 = RoteM.ptr<double>(1)[0];
                double r22 = RoteM.ptr<double>(1)[1];
                double r23 = RoteM.ptr<double>(1)[2];
                double r31 = RoteM.ptr<double>(2)[0];
                double r32 = RoteM.ptr<double>(2)[1];
                double r33 = RoteM.ptr<double>(2)[2];
                //compute the rotational angle for each axis
                double thetaz = atan2(r21, r11) / CV_PI * 180;
                double thetay = atan2(-1 * r31, sqrt(r32 * r32 + r33 * r33)) / CV_PI * 180;
                double thetax = atan2(r32, r33) / CV_PI * 180;
                ​//Now we have the rotational angle, we need to calulate the translation on each axis
                double tx = tvec.ptr<double>(0)[0];
                double ty = tvec.ptr<double>(0)[1];
                double tz = tvec.ptr<double>(0)[2];
                //compute the relative value on the rotated axis
                CodeRotateByZ(tx, ty, -1 * thetaz, x, y);
                CodeRotateByY(tx, tz, -1 * thetay, x, z);
                CodeRotateByX(ty, tz, -1 * thetax, y, z);
                ​
                void CodeRotateByZ(double tx, double ty, double thetaz, double &outx, double &outy)
                {
                  double x1 = tx;
                  double y1 = ty;
                  double rz = thetaz * CV_PI / 180;
                  outx = cos(rz) * x1 - sin(rz) * y1;
                  outy = sin(rz) * x1 + cos(rz) * y1;
                }
                ​
                void CodeRotateByY(double tx, double tz, double thetay, double &outx, double &outz)
                {
                  double x1 = tx;
                  double z1 = tz;
                  double ry = thetay * CV_PI / 180;
                  outx = cos(ry) * x1 + sin(ry) * z1;
                  outz = cos(ry) * z1 - sin(ry) * x1;
                }
                void CodeRotateByX(double ty, double tz, double thetax, double &outy, double &outz)
                {
                  double y1 = ty;
                  double z1 = tz;
                  double rx = thetax * CV_PI / 180;
                  outy = cos(rx) * y1 - sin(rx) * z1;
                  outz = cos(rx) * z1 + sin(rx) * y1;
                }
                ​
                x = x * -1;
                y = y * -1;
                z = z * -1 - 0.1;//the camera is 0.1 m above the kobuki
                printf("The Aruco's location in the world reference frame is %lf %lf %lf", x, y,z);
                // draw axis for each marker
                for(int i=0; i<ids.size(); i++)
                    cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
            }
            cv::imshow("out", imageCopy);
            char key = (char) cv::waitKey(waitTime);
            if (key == 27)
                break;
        }
//TEST - ***************my part ENDS here*****************
        if (store == 1) {
            ros::Subscriber id = nh.subscribe("/test1/markers_test_id", 1, markIdCallback);
            std::cout << " for store marker id" << std::endl;
​
            ros::Subscriber pose = nh.subscribe("/amcl_pose", 1, PoseCallback);
            std::cout << " for store the location" << std::endl;
​
            //ros::spin();
​
​
            ros::Rate loop_rate(1);
            while (ros::ok())
            {
                /*...TODO...*/
​
                ros::spinOnce();
                loop_rate.sleep();
            }
​
​
            //ros::Subscriber a = nh.subscribe("/test1/markers_test_id",10, IdStore);
            //printf("%d", &a);
​
​
        } else {
            printf("please input the valid instruction(input 10 for quit)");
            scanf("%d", &store);
            continue;
        }
​
​
    }
    return 0;
}

```
