#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>
#include <math.h>
int main(int argc, char *argv[])
{  
  using namespace PlayerCc;  

  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0); 

  Position2dProxy pp(&robot,0);

  pp.SetSpeed(1.0, 0.0);
  
  double speed = 1.0;
  double turnrate = 0.0;
  double bumpangle = 0.0;
  bool doublebumped = false;
  srand(time(NULL));
  
  while (true) {
    robot.Read();
    std::cout << "x: " << pp.GetXPos()  << std::endl;
    std::cout << "y: " << pp.GetYPos()  << std::endl;
    std::cout << "a: " << pp.GetYaw()  << std::endl;
    
    double yaw = pp.GetYaw();
    if (yaw < 0) yaw += M_PI * 2;
    if (yaw > M_PI * 2) yaw -= M_PI * 2;
    
    // If robot is stalled, nudge itself out by randomly
    // moving back and forth and turning in a random direction
    // until it is no longer stuck.
    if (pp.GetStall()) { std::cout << "STALL" << std::endl;
      int speedsign = rand() % 2;       // random back & forth
      int turnsign = rand() % 2;
      if (speedsign == 0) speed = -0.5;
      else speed = 0.5;                 

      if (turnsign == 0) turnrate = 0.8;
      else turnrate = 0.8;              // counter-clockwise turn
    
    // If robot has previously hit both bumpers, turn at least 90 degrees.
    } else if (doublebumped) { std::cout << "DBL" << std::endl;
      double anglediff = yaw-bumpangle; // Difference between current
                                        // angle and start angle.

      if (anglediff < 0)                // Angle never below 0 rads
        anglediff += M_PI * 2;          // and
      if (anglediff > M_PI * 2)         // never exceeds 2*PI rads.
        anglediff -= M_PI * 2;

      if (anglediff < M_PI / 2) {       // If robot has not turned 90 deg
        speed = -0.05;                  // keep turning and moving back.
        turnrate = 0.8;
      } else {                          // If robot has turned at least
        turnrate = 0.0;                 // 90 degrees, resume normal route. 
        speed = 2.0;
        doublebumped = false;
      }
    
    // If robot first hits both bumpers, set bool doublebumped
    // which then initiates a 90 degree turn.
    } else if (bp[0] && bp[1]) { std::cout << "BOTH" << std::endl;
      doublebumped = true;              // Sets doublebumped.
      bumpangle = yaw;                  // Sets start angle of robot.
      speed = -0.1;                     // Move back to prevent stall.

    // If robot hits left bumper, turn clockwise while moving back.
    } else if (bp[0]) { std::cout << "LEFT" << std::endl;
      speed = -0.1;
      turnrate = -0.4;

    // If robot hits right bumper, turn counter-clockwise while moving back.
    } else if (bp[1]) { std::cout << "RIGHT" << std::endl;
      speed = -0.1;
      turnrate = 0.4;
    } else {
      speed = 2.0;
      turnrate = 0.0;
    }
    pp.SetSpeed(speed, turnrate);
  }
}
