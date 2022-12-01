#ifndef NAVIGATION_H
#define NAVIGATION

#include "utility.h"

#include <StackArray.h>

enum bot_dir {
  NORTH,
  EAST,
  SOUTH,
  WEST
};

struct pos {
  int x;
  int y;
  bot_dir dir;
};

const pos initial_pos = {
  4,
  0,
  NORTH
};

bool in_stack[25];
bool visited[25];
int  neighbors[25][4]; // ordered NORTH, SOUTH, EAST, WEST
int  treasure[2];

// squares are numbered 0 to 24, starting from the bottom left
// X increasing towards the right
// Y increasing upwards
// robot starts in (4, 0)
int square_num(pos p) {
  return p.x + 5*p.y;
}

bool found_2_treasures() {
  return treasure[0] != 0 && treasure[1] != 0 && treasure[0] != treasure[1];
}

void update_treasure(double freq) {
  if(treasure[0] == -1) {
    treasure[0] = freq;
  } else {
    treasure[1] = freq;
  }
}

pos nav_forward(pos p0) {
  switch(p0.dir) {
    case NORTH:
      if(p0.y == 4) {
        throw_error("nav_forward() tried to move NORTH out of bounds");
      } else p0.y++;
      break;
    case EAST:
      if(p0.x == 4) {
        throw_error("nav_forward() tried to move EAST out of bounds");
      } else p0.x++;
      break;
    case SOUTH:
      if(p0.y == 0) {
        throw_error("nav_forward() tried to move SOUTH out of bounds");
      } else p0.y--;
      break;
    case WEST:
      if(p0.x == 0) {
        throw_error("nav_forward() tried to move WEST out of bounds");
      } else p0.x--;
      break;
    default:
      throw_error("nav_forward() recieved unknown dir: " + String(p0.dir));
      break;
  }

  return p0;
}

pos nav_turnR(pos p0) {
  switch(p0.dir) {
    case NORTH:
      p0.dir = EAST;
      break;
    case EAST:
      p0.dir = SOUTH;
      break;
    case SOUTH:
      p0.dir = WEST;
      break;
    case WEST:
      p0.dir = NORTH;
      break;
    default:
      throw_error("nav_turnR() recieved unknown dir: " + String(p0.dir));
  }

  return p0;
}

pos nav_turnL(pos p0) {
  switch(p0.dir) {
    case NORTH:
      p0.dir = WEST;
      break;
    case EAST:
      p0.dir = NORTH;
      break;
    case SOUTH:
      p0.dir = EAST;
      break;
    case WEST:
      p0.dir = SOUTH;
      break;
    default:
      throw_error("nav_turnL() recieved unknown dir: " + String(p0.dir));
  }

  return p0;
}

void mark_accessible(int s0, int s1, bot_dir dir_from_s0) {
  switch(dir_from_s0) {
    case NORTH:
      neighbors[s0][0] = s1;
      neighbors[s1][2] = s0;
      break;
    case EAST:
      neighbors[s0][1] = s1;
      neighbors[s1][3] = s0;
      break;
    case SOUTH:
      neighbors[s0][2] = s1;
      neighbors[s1][0] = s0;
      break;
    case WEST:
      neighbors[s0][3] = s1;
      neighbors[s1][1] = s0;
      break;
    default:
      throw_error("mark_accessible() recieved unknown dir: " + String(dir_from_s0));
  }
}

void navigation_setup() {
  for(int i = 0; i < 25; i++) {
    in_stack[i] = false;
    visited[i] = false;

    for(int j = 0; j < 4; j++) {
      neighbors[i][j] = -1;
    }
  }

  treasure[0] = -1;
  treasure[1] = -1;
}

#endif
