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
  double adjustangle = dtor(4);
  bool doublebumped = false;
  srand(time(NULL));
  
  // Array listing whether or not each of the four quadrants have been visited.
  // Sets a quadrant to true if the robot has entered the quadrant.
  bool visited[4] = { false };
  
  // Sets the original location of the robot. 
  // If all four quadrants have been hit, and robot enters within a certain
  // distance to its original location, the robot should stop.
  robot.Read();
  double yaw;
  double currXPos, startXPos = pp.GetXPos();
  double currYPos, startYPos = pp.GetYPos();
  double proximity = 3.0;               // Set the proximity for the goal area.
 
  // Sets the original quadrant
  int currQuadrant, startQuadrant;
  if (currXPos > 6 && currYPos > 6) startQuadrant = 0;
  if (currXPos < 6 && currYPos > 6) startQuadrant = 1;
  if (currXPos < 6 && currYPos < 6) startQuadrant = 1;
  if (currXPos > 6 && currYPos < 6) startQuadrant = 1;
 
  while (true) {
    robot.Read();
    currXPos = pp.GetXPos();
    currYPos = pp.GetYPos();
    yaw = pp.GetYaw();
    if (yaw < 0) yaw += M_PI * 2;       // 0 <= yaw <= M_PI*2
    if (yaw > M_PI * 2) yaw -= M_PI * 2;

    std::cout << "x: " << currXPos  << std::endl;
    std::cout << "y: " << currYPos  << std::endl;
    std::cout << "a: " << yaw << std::endl << std::endl;

    // Sets current quadrant.
    // Quadrant I
    if (currXPos > 6 && currYPos > 6) currQuadrant = 0;
    // Quadrant II
    if (currXPos < 6 && currYPos > 6) currQuadrant = 1;
    // Quadrant III
    if (currXPos < 6 && currYPos < 6) currQuadrant = 2;
    // Quadrant IV
    if (currXPos > 6 && currYPos < 6) currQuadrant = 3;

    // Sets current quadrant as visited.
    visited[currQuadrant] = true;
    
    // Manhattan distance
    double mandist = abs(currXPos-startXPos) + abs(currYPos-startYPos);
    // Euclidian distance
    double dist = sqrt(mandist * mandist);

    // If all four quadrants have been visited and robot has entered
    // the goal area, stop.    
    if (visited[0] && visited[1] && visited[2] && visited[3] && dist <= proximity) {
      speed = 0.0;
      turnrate = 0.0;
      break;
    }

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


      if (anglediff < M_PI / 2 - adjustangle) {       // If robot has not turned 90 deg
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
      speed = -0.5;                     // Move back to prevent stall.

    // If robot hits left bumper, turn clockwise while moving back.
    } else if (bp[0]) { std::cout << "LEFT" << std::endl;
      speed = -0.1;
      turnrate = -0.4;

    // If robot hits right bumper, turn counter-clockwise while moving back.
    } else if (bp[1]) { std::cout << "RIGHT" << std::endl;
      speed = -0.5;
      turnrate = 0.4;
    } else {
      speed = 2.0;
      turnrate = 0.0;
    }
    pp.SetSpeed(speed, turnrate);
  }
}
