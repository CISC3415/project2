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
    
    if (pp.GetStall()) {
      std::cout << "STALLED" << std::endl;

      int sign = rand() % 2;
      if (sign == 0) speed = -0.5;
      else speed = 0.5;
      std::cout << sign << std::endl;
      turnrate = 0.8;
    } else if (doublebumped) {
      std::cout << "dblbump" << std::endl;
      double anglediff = yaw-bumpangle;
      if (anglediff < 0) anglediff += M_PI * 2;
      if (anglediff > M_PI * 2) anglediff -= M_PI * 2;
      if (anglediff < M_PI / 2) {
        speed = -0.05;
        turnrate = 0.8;
      } else {
        turnrate = 0.0;
        speed = 2.0;
        doublebumped = false;
      }
    } else if (bp[0] && bp[1]) {
      // turn 90 degrees
      doublebumped = true;
      std::cout << "DOUBLE BUMP" << std::endl;
      bumpangle = yaw;
      speed = -0.1;
    } else if (bp[0]) {
      std::cout << "LEFT BUMP" << std::endl;
      speed = -0.5;
      turnrate = -0.8;
    } else if (bp[1]) {
      std::cout << "RIGHT BUMP" << std::endl;
      turnrate = 0.8;
    } else {
      speed = 2.0;
      turnrate = 0.0;
    }
    pp.SetSpeed(speed, turnrate);
  }
}
