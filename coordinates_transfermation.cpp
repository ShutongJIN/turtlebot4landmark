/*****************************
Use this cpp file to subscribe the information and store it as array
Contributed by Ke GUO, Shutong JIN
 ********************************/
​
#include <iostream>
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
                z = z * -1 + 0.1;//the camera is 0.1 m above the kobuki
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
