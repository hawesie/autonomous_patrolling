# The autonomous patrolling package
           

This package allows for the sequential or randomized visit of a set of pre-defined waypoints in the environment, along with a charging behaviour when the battery level drops below a given treshold. It also has two tools for creating and saving a map, and for creating and saving a set of waypoints.
It assumes that either a [morse simulation](https://github.com/strands-project/strands_morse) or the [scitos_bringup](https://github.com/strands-project/scitos_robot) have been launched


## The map_saver

### Without rumblepad

Prior to waypoint autonomous patrolling, a map of the area to be patrolled needs to be created. For that:

1. Have the robot docked (the docking station must be the origin of the odometry frame)

2. Run the `map_saver` package:
  
      $ roslaunch map_saver ap_map_saver.launch 
        
        
The `ap_map_saver` provides a `SaveMap` service, that takes as input a string of the form `"path to folder where to save the map"/map_name` (e.g., `/home/strands_user/map`) and creates the  `map_name.yaml` and `map_name.pgm` files in the selected folder.


### With rumblepad
  
The `ap_map_saver` can also be used in conjunction with the rumblepad, where the user can build a map by driving the robot around and then save it. To do this:

* Have the robot docked (the docking station must be the origin of the odometry frame)     

* Before launching `ap_map_saver.launch`, launch the teleop_app:
        
           $ roslaunch scitos_teleop teleop_joystick.launch js:=/dev/input/"joystick name"
                
* Launch the `ap_map_saver`, with an optional argument:
        
           $ roslaunch map_saver ap_map_saver.launch map:="path to folder where to save the map"/map_name
                
* (Optional) Start rviz to visualize the map creation: 
     
           $ rosrun rviz rviz
                
* Drive the robot around using the rumblepad
     
* To save the map,  press the (A) button.
       
    * The optional `map` argument sets the path to save the map. This string has the same format as the one used for the `SaveMap` service. It's default value is `~/map`.


    
## The waypoint recorder


### Without rumblepad

Prior to waypoints autonomous patrolling, a file with the waypoints to be visited needs to be created. For that, the `waypoint_recorder` package can be used:
  
      $ roslaunch waypoint_recorder waypoint_recorder map:="file path to the map's .yaml file"
        
        
The `waypoint_recorder` provides two services:
  
* A `SaveWaypoint` service, that saves the next pose published by amcl in a list.

* A `SaveWaypointFile` service, that receives a string specifying the file path of the output file (e.g., `/home/strands_user/waypoints.csv`), and writes all the waypoints saved in the list to that file.





### With rumblepad
  
The `waypoint_recorder` can also be used in conjunction with the rumblepad, where the user can  drive the robot around, create waypoints, and then save them to a file. To do this:
     
* Before launching `waypoint_recorder.launch`, launch the teleop_app:
        
           $ roslaunch scitos_teleop teleop_joystick.launch js:=/dev/input/"joystick name"
                
* Launch the `waypoint_recorder`, with an optional argument:
        
           $ roslaunch waypoint_recorder waypoint_recorder.launch map:="file path to the map's .yaml file" waypoints:="file path to the file where waypoints shoud be saved"
                
* (Optional) Start rviz to check if the robot is well localized, and give it a pose estimate if needed: 
     
           $ rosrun rviz rviz
                
* Drive the robot around using the rumblepad
     
* To save waypoints to the list,  press the (A) button. NOTE: The FIRST point you save should be the one in front of your charging station, so that the robot knows where to navigate when it needs to charge.

* To save the list of waypoints to a file,  press the (B) button.
       
    * The optional `waypoints` argument sets the file path to save the waypoints. This string has the same format as the one used for the `SaveWaypointFile` service. It's default value is `~/waypoints.csv`.



## The waypoint patroller

Aunonomously  visits a pre-defined list of points randomly or in sequence. Goes to charge when battery drops below a given treshold and after it is recharged, continues the patrolling. Assumes static map and waypoints set are given as input.

* Launch the scitos 2d navigation:

           $ roslaunch scitos_2d_navigation scitos_2d_nav.launch map:="file path to the map's .yaml file"
           
  * The 2d navigation is kept separated from the autonomous patroller so one can kill the patrolling process without killing the navigation related nodes.

           
* (Optional) Start rviz to check if the robot is well localized, and give it a pose estimate if needed: 
     
           $ rosrun rviz rviz
      
* Launch the mongodb_store:

           $ HOSTNAME=bob roslaunch mongodb_store datacentre.launch


    * If you haven't inserted waypoints in your datacentre, you can insert the waypoints from a waypoint log file (as created using the waypoint recorder) into your datacentre:
           
               $ rosrun waypoint_recorder insert_in_db.py waypoints.csv point_set_name map_name
          
           Currently map_name is unimportant (until the maps are also stored in db), and the first waypoint is assumed to be the pre-charging waypoint. 






* Run the autonomous docking service:

           $ roslaunch scitos_docking charging.launch
           

    * If this is your first run, or you changed the robot station's location, you need to run the docking calibration:
    
               $ rosrun actionlib axclient.py /chargingServer

                      
      Then in the `Goal` textfield complete as follows:
      
               Command: calibrate
               Timeout: 1000

               
      Then press the `SEND GOAL` button.

* Launch the marathon gui, where HOST_IP should be set to an external ip for your machine if you want this to be externally accessible, else can be left blank for 127.0.0.1. E.g.:
           
               $  HOST_IP=10.0.11.158 roslaunch marathon_touch_gui marathon_gui_dependencies.launch
    
     * WARNING: If you are running the marathon gui for the first time, before launching, follow the installation instuctions on https://github.com/strands-project/strands_ui
     
     * After launching, open a browser window and navigate to `HOST_IP:8090`, where HOST_IP is the one you just set above


* Launch the patroller:
  
           $ roslaunch waypoint_patroller long_term_patroller.launch waypoints:=waypoint_set_name <randomized:="value"> <n_it:="number of iterations">

           
   * The optional argument randomized can be true or false. Default is true. If false is given, then the points are visited sequentially
   * The optional argument n_it specifies how many complete iterations of all the points should be done before the patroller outputs succeeded. Default is -1, which means infinite iterations
   * NOTE: For the robot to speak and ask for help, the user that launches this file needs to be logged in the robot's computer. This is related to [this](https://github.com/strands-project/strands_hri/issues/7)
   * If you have the gamepad running, you can pause the patroller and assume control by pressing the dead-man switch. When the dead-man switch is released the patroller resumes patrolling the previous waypoint.
   * You can also pause execution and regain the possibility to control the robot via other sources (e.g. rviz) by running:

               $ rosservice call /pause_resume_patroller
           
           To resume, just send another service call as above


