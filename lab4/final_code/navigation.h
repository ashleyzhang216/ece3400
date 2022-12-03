#ifndef NAVIGATION_H
#define NAVIGATION

#include "utility.h"

#define MAZE_X 3
#define MAZE_Y 3

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
  MAZE_X - 1,
  0,
  NORTH
};

bool in_stack[MAZE_X * MAZE_Y];
bool visited[MAZE_X * MAZE_Y];
int  neighbors[MAZE_X * MAZE_Y][4]; // ordered NORTH, EAST, SOUTH, WEST
int  treasure[2];

// squares are numbered 0 to 24, starting from the bottom left
// X increasing towards the right
// Y increasing upwards
// robot starts in (4, 0)
int square_num(pos p) {
  return p.x + MAZE_Y*p.y;
}

int square_num(int x, int y) {
  return x + MAZE_Y * y;
}

bool found_2_treasures() {
  return treasure[0] != -1 && treasure[1] != -1 && treasure[0] != treasure[1];
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
      if(p0.y == MAZE_Y - 1) {
        throw_error("nav_forward() tried to move NORTH out of bounds");
      } else p0.y++;
      break;
    case EAST:
      if(p0.x == MAZE_X - 1) {
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

int neighbor(pos s0, bot_dir dir) {
  switch(dir) {
    case NORTH:
      if(s0.y != MAZE_Y - 1) {
        s0.y++;
        return square_num(s0);
      } else return -1;
    case EAST:
      if(s0.x != MAZE_X - 1) {
        s0.x++;
        return square_num(s0);
      } else return -1;
    case SOUTH:
      if(s0.y != 0) {
        s0.y--;
        return square_num(s0);
      } else return -1;
    case WEST:
      if(s0.x != 0) {
        s0.x--;
        return square_num(s0);
      } else return -1;
    default:
      throw_error("neighbor() recieved unknown dir: " + String(dir));
  }
}

bot_dir dir_from_bot(pos s0, int us_index) {
  switch(us_index) {
    case 0: 
      return nav_turnL(s0).dir;
    case 1: 
      return s0.dir;
    case 2: 
      return nav_turnR(s0).dir;
    default:
      throw_error("dir_from_bot() recieved unknown us_index: " + String(us_index));
  }
}

void mark_inaccessible(pos s0, int us_index) {
  bot_dir dir_from_s0 = dir_from_bot(s0, us_index);
  int s1 = neighbor(s0, dir_from_s0);
  switch(dir_from_s0) {
    case NORTH:
      neighbors[square_num(s0)][0] = -1;
      if(s1 != -1) {
        neighbors[s1][2] = -1;
      }
      break;
    case EAST:
      neighbors[square_num(s0)][1] = -1;
      if(s1 != -1) {
        neighbors[s1][3] = -1;
      }
      break;
    case SOUTH:
      neighbors[square_num(s0)][2] = -1;
      if(s1 != -1) {
        neighbors[s1][0] = -1;
      }
      break;
    case WEST:
      neighbors[square_num(s0)][3] = -1;
      if(s1 != -1) {
        neighbors[s1][1] = -1;
      }
      break;
    default:
      throw_error("mark_inaccessible() recieved unknown dir: " + String(dir_from_s0));
  }
}

bool is_accessible(int pos0, int pos1) {
  return neighbors[pos0][0] == pos1 && neighbors[pos1][2] == pos0 || 
         neighbors[pos0][1] == pos1 && neighbors[pos1][3] == pos0 || 
         neighbors[pos0][2] == pos1 && neighbors[pos1][0] == pos0 || 
         neighbors[pos0][3] == pos1 && neighbors[pos1][1] == pos0;
}

pos turn_towards(pos pos0, int pos1) {
  
  int num_turns;

  for(int x = 0; x < 4; x++) {
    Serial.print(String(neighbors[square_num(pos0)][x]) + ", ");
  }
  Serial.println();
  
  if(neighbors[square_num(pos0)][0] == pos1) {
    num_turns = pos0.dir - NORTH;
    Serial.print("0: ");
  } else if(neighbors[square_num(pos0)][1] == pos1) {
    num_turns = pos0.dir - EAST; 
    Serial.print("1: ");
  } else if(neighbors[square_num(pos0)][2] == pos1) {
    num_turns = pos0.dir - SOUTH;
    Serial.print("2: ");
  } else if(neighbors[square_num(pos0)][3] == pos1) {
    num_turns = pos0.dir - WEST;
    Serial.print("3: ");
  } else {
    throw_error("travel_to() got non-neigbor pos1: " + String(pos1));
  }

  if(num_turns < 0) num_turns += 4;
  Serial.println(num_turns);

  if(num_turns == 3) {
    turnR();
    pos0 = nav_turnR(pos0);
  } else {
    for(int i = 0; i < num_turns; i++) {
      turnL();
      pos0 = nav_turnL(pos0);
    }
  }

  return pos0;
}

pos travel_to(pos pos0, int pos1) {
  pos0 = turn_towards(pos0, pos1);
  move_forward();
  return nav_forward(pos0);
}

void navigation_setup() {
  for(int i = 0; i < MAZE_X * MAZE_Y; i++) {
    in_stack[i] = false;
    visited[i] = false;
  }
  
  for(int i = 0; i < MAZE_X; i++) {
    for(int j = 0; j < MAZE_Y; j++) {
      neighbors[square_num(i, j)][0] = (j == MAZE_Y - 1) ? -1 : square_num(i, j + 1);
      neighbors[square_num(i, j)][1] = (i == MAZE_X - 1) ? -1 : square_num(i + 1, j);
      neighbors[square_num(i, j)][2] = (j == 0)          ? -1 : square_num(i, j - 1);
      neighbors[square_num(i, j)][3] = (i == 0)          ? -1 : square_num(i - 1, j);
    }
  }

  treasure[0] = -1;
  treasure[1] = -1;
}

#endif
