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

    if (bp[0] || bp[1] || pp.GetStall()) {
      std::cout << "BUMPED!" << std::endl; 
      for (int i = 0; i < 120; i++) {
        pp.SetSpeed(-0.5, 0.0);
      }
      if (dis < 6.0) {
        while (abs(rtod(yaw)-rtod(tta)) > 3.00) {
          std::cout << "dist: " << rtod(tta) << std::endl;
          std::cout << "yaw : " << rtod(yaw) << std::endl;
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += (M_PI*2);
          if (yaw > M_PI*2) yaw -= (M_PI*2);
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
          std::cout << "curr: " << yaw << std::endl;
          std::cout << "goal: " << start_yaw << std::endl;
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += M_PI*2;
          if (yaw > M_PI*2) yaw -= (M_PI*2);
          pp.SetSpeed(0.0, 0.4);
        }
      } else {
        tta += M_PI;
	if (tta > (2*M_PI)) tta -= (2*M_PI);
        double diff = abs(rtod(yaw)-rtod(tta));
        while (abs(rtod(yaw)-rtod(tta)) > 3.00) {
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += (M_PI*2);
          if (yaw > M_PI*2) yaw -= (M_PI*2);
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
          std::cout << "curr: " << yaw << std::endl;
          std::cout << "goal: " << start_yaw << std::endl;
          robot.Read();
          yaw = pp.GetYaw();
          if (yaw < 0) yaw += M_PI*2;
          if (yaw > M_PI*2) yaw -= (M_PI*2);
          pp.SetSpeed(0.0, -0.4);
        }
      }
    }
    
    // randomly bump into things
    
    int r = rand() % 69 + 1;
    std::cout << r << std::endl;
    bool bumped = false;
    if (r == 1) {
      int riter = rand() % 15 + 15;
      double rspeed = (double)rand() / RAND_MAX;
      rspeed = (rspeed + 1.5) * (1.5);
      double rturnrate = (double)rand() / RAND_MAX;
      rturnrate *= 2*M_PI;
      std::cout << rspeed << ", " << rturnrate << std::endl;
      
      for (int i = 0; i < riter; i++) {
        if (bp[0] || bp[1]) {
          for (int i = 0; i < 10; i++) {
            robot.Read();
            pp.SetSpeed(-0.5, 0.0);
          }
          break;
        }
        robot.Read();
        pp.SetSpeed(rspeed, rturnrate);
      }
      
    }
    pp.SetSpeed(3.0, 1.0/2.0);
  }
}


