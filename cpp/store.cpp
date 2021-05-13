/*****************************
Use this cpp file to subscribe the information and store it as array
Created by Ke and Shutong
 ********************************/

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




