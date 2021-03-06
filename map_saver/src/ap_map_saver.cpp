#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>
#include <ros/console.h>

#include "ap_msgs/SaveMap.h"

#include <stdlib.h>
#include <string.h>

#if WITH_TELEOP
	#include "scitos_apps_msgs/action_buttons.h"
#endif


// #if WITH_TELEOP
// 	void buttonCallback(const scitos_apps_msgs::action_buttons::ConstPtr& msg)
// 	{
// 	  if(msg->A) {
// 		std::string command("rosrun map_server map_saver -f ");
// 		command += map_name;
// 		ROS_INFO("Saving map as: %s", map_name.c_str());
// 		system(command.c_str());
// 	  }
// 	}
// #endif


std::string map_name;

bool saveMap(std::string file_name){
  	std::string command("rosrun map_server map_saver -f ");
	command += file_name;
	ROS_INFO("Saving map as: %s", command.c_str());

	system(command.c_str());
	return true;
}



#if WITH_TELEOP
	void buttonCallback(const scitos_apps_msgs::action_buttons::ConstPtr& msg)
	{
	  if(msg->A) {
		saveMap(map_name);
	  }
	}
#endif


bool saveMapSrv(ap_msgs::SaveMap::Request  &req, ap_msgs::SaveMap::Response &res)
{
	saveMap(req.file_name);
	return true;
}	


  

int main(int argc, char **argv)
{



  
  
  
  
  /**  ROS_INFO("Map name: %s", map_name.c_str());
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line. For programmatic
   * remappings you can use a different version of init() which takes remappings
   * directly, but for most command-line programs, passing argc and argv is the easiest
   * way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "ap_map_saver");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n("ap_map_saver");
  
  



  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to t	ROS_INFO("Saving map as: %s", file_name.c_str());
he subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */
#if WITH_TELEOP
  	  ros::Subscriber sub = n.subscribe("/teleop_joystick/action_buttons", 1000, buttonCallback);
	  //Check if map name was given as argument to the launch file, and create default map name otherwise
	  map_name=std::string(argv[1]);
	  if (!map_name.compare(std::string("default_map_name")))  {
	    ROS_WARN("No file name given for map, map will be saved with default name on home directory");
	    std::string home(getenv("HOME"));
	    home+="/";
	    char buff[20];
	    time_t now = time(NULL);
	    strftime(buff, 20, "%Y_%m_%d_%H_%M_%S", localtime(&now));
	    map_name = std::string("~/") + std::string(buff) + std::string("map");
	  }
#endif
  ros::ServiceServer service = n.advertiseService("SaveMap", saveMapSrv);

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */
  ros::spin();

  return 0;
}
