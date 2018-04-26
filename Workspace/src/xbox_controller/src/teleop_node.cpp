#include "ros/ros.h"
#include "sensor_msgs/Joy.h"
#include "std_msgs/String.h"

/*
This code implements an algorithm that cycles through different branches when a
button is pressed.
The button used to switch between branches is the CENTER XBOX BUTTON. (The big
circular button with an X).

When in drive mode, hold a to activate the joysticks and turn as desired,
alternatively, use the rb button to enter turbo mode and go quicker 

The order of cycle goes like : Drive --> Arm --> Science --> Autonomy --> Drive......

*/

// A class that contains publishers and subscribers as the data members, and the
// call-back function as the member function

class StateSelect {
public:
  void callback(const sensor_msgs::Joy::ConstPtr &joy);

  StateSelect() { // Constructor
    // Subscriber
    joy_sub =
        n.subscribe<sensor_msgs::Joy>("joy", 1, &StateSelect::callback, this);

    // Publisher to four separate branches
    drive_pub = n.advertise<sensor_msgs::Joy>("drive_joy", 1);
    science_pub = n.advertise<sensor_msgs::Joy>("science_joy", 1);
    arm_pub = n.advertise<sensor_msgs::Joy>("arm_joy", 1);
    autonomy_pub = n.advertise<sensor_msgs::Joy>("autonomy_joy", 1);

    // Logic Variables Initialization
    i = DRIVE;       // Starts at the drive branch
    pressed = false; // Initially, the change button is assumed to be NOT
                     // PRESSED

    // populate timestamp, axes and buttons of empty joy msg, make into
    // function?
    empty_joy_msg.header.stamp = ros::Time().now();

    // get size of buttons and axes arrays
    int num_buttons = empty_joy_msg.buttons.size();
    int num_axes = empty_joy_msg.axes.size();

    // loop through each set, set all values to 0
    for (int j = 0; j < num_buttons; j++) {
      empty_joy_msg.buttons[j] = 0;
    }

    for (int k = 0; k < num_axes; k++) {
      empty_joy_msg.axes[k] = 0.0;
    }
  }

private:
  ros::NodeHandle n;

  ros::Subscriber joy_sub;
  ros::Publisher drive_pub;
  ros::Publisher science_pub;
  ros::Publisher arm_pub;
  ros::Publisher autonomy_pub;
  enum topics { DRIVE, ARM, SCIENCE, AUTONOMY, NUM_TOPICS };
  int i; // Variable thats holds the current state index
  bool pressed;
  sensor_msgs::Joy
  empty_joy_msg; // empty joy message needed when switching states
};

// Callback function: Called everytime there is an update on the joy-stick
void StateSelect::callback(const sensor_msgs::Joy::ConstPtr &joy) {

  // logic to cycle through different states
  if (joy->buttons[8] == 1 && !pressed) {
    // Send empty message
    if (i == DRIVE)
      drive_pub.publish(empty_joy_msg);

    else if (i == ARM)
      arm_pub.publish(empty_joy_msg);

    else if (i == SCIENCE)
      science_pub.publish(empty_joy_msg);

    else if (i == AUTONOMY)
      autonomy_pub.publish(empty_joy_msg);

    i++;
    if (i >= NUM_TOPICS)
      i = DRIVE; // Cycle back to drive after autonomy branch
    pressed = true;
  } else
    pressed = false;

  // The algorithm below is required to counter controller debouncing.
  if (joy->buttons[8] != 1)
    pressed = false;
  else
    pressed = true;

  if (i == DRIVE)
    drive_pub.publish(joy);

  else if (i == ARM)
    arm_pub.publish(joy);

  else if (i == SCIENCE)
    science_pub.publish(joy);

  else if (i == AUTONOMY)
    autonomy_pub.publish(joy);
}

int main(int argc, char *argv[]) {
  // Initializes Node
  ros::init(argc, argv, "teleop_node");

  StateSelect state_obj;

  ros::spin();
}
