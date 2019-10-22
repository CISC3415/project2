/*
 *  CISC-3415 Robotics
 *  Project 2 - Part 1
 *  Date: 10th October 2019
 *
 ** Group Members *************************************************************
 *    
 *  Benjamin Yi
 *  Emmanuel Desdunes
 *  Montasir Omi
 *  Shahzad Ahmad
 *
 ** Description ***************************************************************
 * 
 *  A program which instructs a simulated robot to roam the simulated
 *  world by only using its sensors (bumpers). The world is mostly clear
 *  except for a giant rock looking object anchored to the center.
 *  The goal of the robot is to use its bumper to navigate to all four
 *  quadrants of the map. Once it enters the vicinity of its original location,
 *  it will halt.
 */

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
  int i = 0;                            // Standard iterator
  bool bumped = false;                  // Required to initiate turns
  srand(time(NULL));                    // Required for nudging out
  
  // Array listing whether or not each of the four quadrants have been visited.
  // Sets a quadrant to true if the robot has entered the quadrant.
  bool visited[4] = { false };
  
  // Sets the original location of the robot. 
  // If all four quadrants have been hit, and robot enters within a certain
  // distance to its original location, the robot should stop.
  robot.Read();
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

    std::cout << "x: " << currXPos  << std::endl;
    std::cout << "y: " << currYPos  << std::endl;
    std::cout << "a: " << pp.GetYaw() << std::endl << std::endl;

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
    if (pp.GetStall()) {
      int speedsign = rand() % 2;       // random back & forth
      int turnsign = rand() % 2;        // random turn direction
      if (speedsign == 0) speed = -0.5;
      else speed = 0.5;                 

      if (turnsign == 0) turnrate = 0.8;
      else turnrate = 0.8;              // counter-clockwise turn
    
    // If robot has previously hit both bumpers, turn at least 90 degrees.
    } else if (bumped) {
      if (i < 16) {                     // 0-16 : robot backs up
        speed = -1.0;
        turnrate = 0.0;
      } else if (i < 36) {              // 17-36 : robot turns (~90degrees)
        speed = 0.0;
        turnrate = 0.8;
      } else {
        bumped = false;
        speed = 2.0;
        turnrate = 0.0;
      }
      i++;
    // If robot first hits both bumpers, set bumped
    // which then initiates a 90 degree turn.
    } else if (bp[0] && bp[1]) {
      bumped = true;                    // Sets bumped.
      i = 0;
      speed = -0.5;                     // Move back to prevent stall.
      turnrate = 0.0;

    // If robot hits left bumper, turn clockwise while moving back.
    } else if (bp[0]) {
      int speedsign = rand() % 2;       // Prevents stall
      speed = -0.2;
      if (speedsign == 0) speed *= -1;
      turnrate = -0.4;

    // If robot hits right bumper, turn counter-clockwise while moving back.
    } else if (bp[1]) {
      int speedsign = rand() % 2;       // Prevents stall
      speed = -0.2;
      if (speedsign == 0) speed *= -1;
      turnrate = 0.4;
    } else {
      speed = 2.0;
      turnrate = 0.0;
    }
    pp.SetSpeed(speed, turnrate);
  }
}
