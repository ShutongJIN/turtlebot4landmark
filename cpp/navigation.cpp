/*****************************
Use this cpp file to give navigation goals to the TurTleBot
Created by Shutong JIN
 ********************************/
#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <cstdio>
​
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
​
int main(int argc, char** argv){
  //test-array
  //the first index - id of the ArUco marker
  //the second index - x in the coordinates
  //the third index - y in the coordinates
  int a[6][3]={{0,0,0},{1,1,1},{2,2,2},{3,3,3},{4,-3,-3},{5,0,0}};
​  // we should change the value of a to the real information we detected here
  int marker=0;
  //test-loop
  //the condition can be changed due to the actual circumstance
  while(marker!=10){
    printf("The target markers are %d %d %d \n.",a[1][0],a[2][0],a[3][0]);
    printf("Please input the value of the landmark:\n");
    int temp=marker;//temp is used to save the former value
    //because we are using absolute, not relative, coordinates in this project
    scanf("%d",&marker);
    if(marker==10){
      break;
    }
​
    ros::init(argc, argv, "simple_navigation_goals");
​
    //tell the action client that we want to spin a thread by default
    MoveBaseClient ac("move_base", true);
​
    //wait for the action server to come up
    while(!ac.waitForServer(ros::Duration(5.0))){
      ROS_INFO("Waiting for the move_base action server to come up");
    }
​
    move_base_msgs::MoveBaseGoal goal;
​
    goal.target_pose.header.frame_id = "base_link";
    goal.target_pose.header.stamp = ros::Time::now();
​
    goal.target_pose.pose.position.x = a[marker][1] - a[temp][1];
    goal.target_pose.pose.position.y = a[marker][2] - a[temp][2];
    goal.target_pose.pose.orientation.w = 1.0;
​
    ROS_INFO("Sending goal");
    ac.sendGoal(goal);
​
    ac.waitForResult();
​
    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
      printf("Hooray, the base moved %d meter in the direction of x and %d meter in the direction of y\n", a[marker][1],a[marker][2]);
    else
      printf("The base failed to move for some reason\n");
  }
  printf("Voila! This is the end!\n");
  return 0;
}
