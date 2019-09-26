/*
 *  bumper.cc
 * 
 *  Sample code for a robot that has two front bumpers and uses them to
 *  avoid obstacles. Suitable for use with the Roomba and Create. 
 * 
 *  Works IRL. Doesn't work so well in Stage, probably because of the
 *  crude modeling of the bumpers, and can get stuck in convex shapes
 *  when it backs into an object while trying to turn away from it.
 *
 *  Based on an example provided by Monica Anderson and Jeff Forbes,
 *  via Carlos Jaramillo, and changed to (hopefully) make it easier to
 *  understand.
 *
 *  Modified:    Simon Parsons
 *  Date:        15th June 2009
 *  Last change: 19th September 2011
 *  
 */

#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>
#include <math.h>

int main(int argc, char *argv[])
{  
  using namespace PlayerCc;  

  // Set up proxies. Proxies are the datastructures that Player uses to
  // talk to the simulator and the real robot.

  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0);       // The bumper proxy reads from the
                                      // bumpers.
  Position2dProxy pp(&robot,0);       // The 2D proxy reads odometry from 
                                      // the robot and sends motion commands.

  // Allow the program to take charge of the motors (take care now)
  pp.SetMotorEnable(true);

  pp.SetSpeed(1.0, 0.0);
  
  double start_x = 0.0;
  double rads = 0.0;
  double t = clock();

  // Find a way to hit the circumference of the circle

  

  /*while (start_x <= 6.0) {
    robot.Read();
    std::cout << "x: " << pp.GetXPos()  << std::endl;
    std::cout << "y: " << pp.GetYPos()  << std::endl;
    std::cout << "a: " << pp.GetYaw()  << std::endl;
    start_x = pp.GetXPos();
     
    pp.SetSpeed(1.0, 0.0);
  }*/
  
  pp.SetSpeed(0.0, 0.0);
  srand(time(NULL));
  while(true) {
    robot.Read();
    double x = pp.GetXPos();
    double y = pp.GetYPos();
    double yaw = pp.GetYaw();
    if (yaw < 0) {
      yaw += (M_PI*2);
    }
    double tta = atan2(y-6, x-6);
    if (tta < 0) {
      tta += (M_PI*2);
    }

    double dis = sqrt(abs((x-6)*(x-6)) + abs((y-6)*(y-6)));
    std::cout << "x: " << pp.GetXPos()  << std::endl;
    std::cout << "y: " << pp.GetYPos()  << std::endl;
    std::cout << "a: " << rtod(yaw)  << std::endl;
    std::cout << "t: " << rtod(tta) << std::endl;
    std::cout << "d: " << dis << std::endl;

    if (bp[0] || bp[1]) {
      std::cout << "BUMPED!" << std::endl; 
      for (int i = 0; i < 120; i++) {
        pp.SetSpeed(-0.5, 0.0);
      }
      if (dis < 6.0) {
        while (abs(rtod(yaw)-rtod(tta)) > 1.00) {
          std::cout << "dist: " << rtod(tta) << std::endl;
          std::cout << "yaw : " << rtod(yaw) << std::endl;
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += (M_PI*2);
          pp.SetSpeed(0.0, 0.5);
        }
        while (dis < 6.0) {
          robot.Read();
          x = pp.GetXPos();
          y = pp.GetYPos();
          dis = sqrt(abs((x-6)*(x-6)) + abs((y-6)*(y-6)));
          pp.SetSpeed(2.0, 0.0);
        }
        double start_yaw = yaw;
        while (yaw - start_yaw < M_PI/2) {
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += M_PI*2;
          pp.SetSpeed(0.0, 0.8);
        }
      } else {
        tta += M_PI;
	if (tta > (2*M_PI)) tta -= (2*M_PI);
        while (abs(rtod(yaw)-rtod(tta)) > 1.00) {
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += (M_PI*2);
          pp.SetSpeed(0.0, 0.5);
        }
        while (dis >= 6.0) {
          robot.Read();
          x = pp.GetXPos();
          y = pp.GetYPos();
          dis = sqrt(abs((x-6)*(x-6)) + abs((y-6)*(y-6)));
          pp.SetSpeed(2.0, 0.0);
        }
        double start_yaw = yaw;
        while (start_yaw - yaw < M_PI/2) {
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += M_PI*2;
          pp.SetSpeed(0.0, -0.8);
        }
      }
    }
    
    // randomly bump into things
    
    int r = rand() % 30 + 1;
    std::cout << r << std::endl;
    bool bumped = false;
    if (r == 1) {
      int riter = rand() % 10 + 16;
      double rspeed = (double)rand() / RAND_MAX;
      rspeed = (rspeed + 1.5) * (1.5);
      double rturnrate = (double)rand() / RAND_MAX;
      rturnrate *= 2*M_PI;
      
      for (int i = 0; i < riter; i++) {
        if (bp[0] || bp[1]) bumped = true;
        std::cout << "GOING " << i << std::endl;
        robot.Read();
        pp.SetSpeed(rspeed, rturnrate);
      }
      if (bumped) {
        std::cout << "BACKUP" << std::endl;
        for (int i = 0; i < 120; i++) {
          robot.Read();
          pp.SetSpeed(-0.5, 0.0);
        }
      }
    }
   
    pp.SetSpeed(3.0, 1.0/2.0);
  }
  // Control loop
  while(true) 
    {    
      double turnrate, speed;

      // Read from the proxies.
      robot.Read();

      // What does odometry tell us? In other words, how far do we
      // think we have gone?
      std::cout << "x: " << pp.GetXPos()  << std::endl;
      std::cout << "y: " << pp.GetYPos()  << std::endl;
      std::cout << "a: " << pp.GetYaw()  << std::endl;


      // Print out what the bumpers tell us:
      std::cout << "Left  bumper: " << bp[0] << std::endl;
      std::cout << "Right bumper: " << bp[1] << std::endl;
      
      // If either bumper is pressed, go backwards. Depending on the
      // combination of bumpers that are pressed, turn some also,
      // trying to turn away from the point of contact. 
      //
      // Otherwise just go forwards
      if(bp[0] || bp[1]){

	speed=-0.5;

	// Left bumper in contact, right bumper not in contact. Turn
	// to the right.  
	//
	// dtor converts from degrees to radians.
	if (bp[0] && !bp[1]) {  
	  turnrate=dtor(-10);  
	}

	// Left bumper not in contact, right bumper in contact. Turn
	// to the left
	if (!bp[0] && bp[1]) {
	  turnrate=dtor(10);
	}

	// Both left and right bumpers in contact. Make a random
	// choice about which way to turn.
	if (bp[0] && bp[1]) {
	  if(rand()%2 > 0){
	    turnrate=dtor(-10);
	  }
	  else {
	    turnrate=dtor(10);
	  }
	}
      } 
      else {
	  turnrate = 0;   
	  speed=0.5;
	}     

      // What did we decide to do?
      std::cout << "Speed: " << speed << std::endl;      
      std::cout << "Turn rate: " << turnrate << std::endl << std::endl;

      // Send the motion commands that we decided on to the robot.
      pp.SetSpeed(speed, turnrate);  
    }
  
}


