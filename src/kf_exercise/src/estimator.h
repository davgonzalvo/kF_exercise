#ifndef ESTIMATOR_NODE_H
#define ESTIMATOR_NODE_H

#include <boost/bind.hpp>
#include <Eigen/Eigen>
#include <stdio.h>

#include <ros/ros.h>
#include <tf/tf.h>

#include <sensor_msgs/Imu.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Quaternion.h>


class EstimatorNode {
 public:
  EstimatorNode();
  ~EstimatorNode();

  //utilities to calibrate some samples of a sensor
  void startCalibration ();
  void endCalibration () ;

  void predict (); 
  void update  (); 

  void publishPose();


 private:

// stores my current best estimate and covariance (can come either from prediction or correction)
  Eigen::VectorXd x_hat; 
  Eigen::MatrixXd P_hat ;

   /* BEGIN: Define here the matrices and vectors of the Kalman filter */

  Eigen::VectorXd u; // input
  Eigen::VectorXd y; // measurement

/* System matrices */
  Eigen::MatrixXd A;
  Eigen::MatrixXd B;
  Eigen::MatrixXd C;
  Eigen::MatrixXd Q;
  Eigen::MatrixXd R;
  
  /* Kalman Gain */
  Eigen::MatrixXd L;


  /* END: Define here the matrices and vectors of the Kalman filter */  

  // parameters to load from parameter server
  double sigma_sqr_P0;
  double sigma_sqr_gps;
  double sigma_sqr_process;


  // subscribers
  ros::Subscriber imu_sub_;
  ros::Subscriber pose_sub_;  
  
  // Publishers  
  ros::Publisher odometry_pub_;
    
  //input messages
  sensor_msgs::Imu                                incomingImuMsg_;
  geometry_msgs::PoseWithCovarianceStamped        incomingPoseMsg_;

  //output messages  
  nav_msgs::Odometry  msgOdometry_;

  //time management
  ros::WallTime   time_reference; 

  // Vectors used to calibrate sensors "calib_"
  bool                        calibrating; 
  std::vector<tf::Quaternion> calib_imu_att_q_buffer;
  tf::Quaternion              imu_att_q_bias;
  std::vector<tf::Vector3>    calib_imu_ang_vel_buffer;
  tf::Vector3                 imu_ang_vel_bias;
  std::vector<tf::Vector3>    calib_imu_accel_buffer;
  tf::Vector3                 imu_accel_bias;
  std::vector<tf::Vector3>    calib_pose_sensor_pos_buffer;
  tf::Vector3                 pose_sensor_pos_offset;
  std::vector<tf::Quaternion> calib_pose_sensor_att_buffer;
  tf::Quaternion              pose_sensor_att_bias;

  tf::Vector3                  gravity_w; // gravity in the world frame

  // rotation from pose1 to imu
  tf::Quaternion              pose2imu_rotation; 
  
  void ImuCallback(
      const sensor_msgs::ImuConstPtr& imu_msg);

  void GPSCallback(
      const geometry_msgs::PoseWithCovarianceStampedConstPtr& pose_msg);

  void readPars(
    ros::NodeHandle& nh);

  //utilities to calibrate some samples of a sensor
  tf::Quaternion  averageQuaternion(std::vector<tf::Quaternion> vec);
  tf::Vector3     averageVector3(std::vector<tf::Vector3> vec);

};

#endif // ESTIMATOR_NODE_H
