cv::Mat Rvec, Tvec;
double rm[9];
RoteM = cv::Mat(3, 3, CV_64FC1, rm);//make sure every pixel has is a 64 bit float
cv::Rodrigues(rvec, RoteM); //obtain the Rodrigues matrix
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

double tx = tvec.ptr<double>(0)[0];
double ty = tvec.ptr<double>(0)[1];
double tz = tvec.ptr<double>(0)[2];
//compute the relative value on the rotated axis
CodeRotateByZ(tx, ty, -1 * thetaz, x, y);
CodeRotateByY(tx, tz, -1 * thetay, x, z);
CodeRotateByX(ty, tz, -1 * thetax, y, z);

void CodeRotateByZ(double tx, double ty, double thetaz, double &outx, double &outy)
{
  double x1 = tx;
  double y1 = ty;
  double rz = thetaz * CV_PI / 180;
  outx = cos(rz) * x1 - sin(rz) * y1;
  outy = sin(rz) * x1 + cos(rz) * y1;
}

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

x = x * -1;
y = y * -1;
z = z * -1 - 0.1;//the camera is 0.1 m above the kobuki
printf("The Aruco's location in the world reference frame is %lf %lf %lf", x, y,z);
