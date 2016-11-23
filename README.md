##Purpose

My team built and programmed a robot to compete for the VEX Robotics 2014-2015 competition Skyrise. Check out some pictures at [http://sameetandpotatoes.github.io/Robotics]()

##Code

There are two parts to the code: autonomous and driver control. 

**Autonomous**: There is a 60 second programming only mode where our robot scores 27 points by stacking five skyrise sections and four blocks. There is also a 15 second autonomous before every match where our robot scores 12 points by stacking three skyrise sections and a block on the skyrise section. A backup autonomous programs scores two blocks on the tiles.

**Driver Control**: Driver control code is where the dual joysticks were mapped to the robot controls. The joysticks had different controls so it required two different drivers at all times to operate. The advantage of this was that it made the robot extremely easy to drive.

##File Structure

- `Auton.c` was our most stable version of the code that we used at the U.S Open VEX Robotics Nationals Championship. It does not contain our code for Programming Skills, where we score 27 points. That can be found in the `Old` folder. 

- The excel spreadsheet is how we made our autonomous program dynamic. Here is how it works: we use a shaft encoder analog sensor to get differences in shaft encoder counts for when the elevator lift raises and lowers the skyrise section. The excel spreadsheet records all of these differences for three trials for the scoring of each section. Then, those differences are averaged, and plotted to correlate a linear relationship between the difference in shaft encoder counts versus the number of skyrise sections.
