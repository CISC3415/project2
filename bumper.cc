#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>

int main(int argc, char *argv[])
{  
  using namespace PlayerCc;  

  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0); 

  Position2dProxy pp(&robot,0);

  pp.SetSpeed(1.0, 0.0);
  
  double speed = 1.0;
  double turnrate = 0.0;
  int bumped = 0;
  
  while (true) {
    robot.Read();
    std::cout << "x: " << pp.GetXPos()  << std::endl;
    std::cout << "y: " << pp.GetYPos()  << std::endl;
    std::cout << "a: " << pp.GetYaw()  << std::endl;


    int minx = -1.9, maxx = 13.9;
    int miny = -1.9, maxy = 13.9;

    if (bp[0] || bp[1]) {
      std::cout << "BUMPED" << std::endl;
      speed = -1.5;
      bumped = 1;
      if (bumped) {
        std::cout << speed << std::endl;
        continue;
      }
      if (bp[0]) {
        turnrate = 0.8;
      } else {
        turnrate = -0.8;
      }
    } else {
      turnrate = 0.0;
      speed = 1.0;
      bumped = 0;
    }
    pp.SetSpeed(speed, turnrate);
  }
}
