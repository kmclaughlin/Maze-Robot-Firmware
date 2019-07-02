# Maze-Robot-Firmware

Arduino firmware for Queen's Robotics Society robot platform for maze solving.

Has both left hand and right hand rule implemetations. The rule to use is activated by checking the distance to the left and right wall. If the left wall is closer LHR is used otherwise RHR. This allows the maze to be checked for the optimal algorithm and the robot to be told the correct algorithm to use by correctly choosing its starting orrientation in the maze.

This implementation uses the encoders on the motors for accurate speed control, providing accurate turns.
