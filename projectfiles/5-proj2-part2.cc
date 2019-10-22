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
  
  double speed = 2.0;
  double turnrate = 0.0;
  int turnsignal = 1;                   // determines left/right turn
  int i = 0;                            // robot progress
  int timesbumped = 0;                  // determines when to turn
  bool bumped = false;                  // determines when to rotate

  while (true) {
    robot.Read();

    std::cout << "x: " << pp.GetXPos() << std::endl;
    std::cout << "y: " << pp.GetYPos() << std::endl;
    std::cout << "a: " << pp.GetYaw() << std:: endl << std::endl;

    // If robot has previously hit a bumper....
    if (bumped) {
      if (i < 16) {                     // Move back ~2 meters
        speed = -1.0;
        turnrate = 0.0; 
     } else if (i < 36) {               // Rotate ~(+/-)90 degrees
        speed = 0.0;
        turnrate = 0.8 * turnsignal;
      } else {                          // Resume normal activity
        bumped = false;
        turnrate = 0.0;
        speed = 2.0;
      }
      i++;
    
    // If robot hits a bumper,
    //   * if it is the 7th bump, it is at the end.
    //   * if it is an even bump, then swap the turn signal
    //   initiate a 90 degree turn
    // set bool bumped
    // which then initiates a 90 degree turn.
    } else if (bp[0] || bp[1]) {
      bumped = true;                    // Set bumped.
      timesbumped++;                   
      if (timesbumped == 7) {           // 7th bumps means robot is at goal.
        pp.SetSpeed(0.0, 0.0);          // STOP.
        break;
      }
      if (timesbumped > 1 && timesbumped % 2 == 1)
        turnsignal *= -1;               // Swap turn signal every 2 bumps.
      speed = -1.0;                     // Start moving back.
      turnrate = 0.0;
      i = 0;
    }
    pp.SetSpeed(speed, turnrate);
  }
}
