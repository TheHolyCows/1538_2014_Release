# Team 1538 2014 FRC Code

## Credits

* Kiet Chau (kiet.chau@team1538.com) - Engineering Mentor
* Connor Worley (connor.worley@team1538.com) - Manager of Software and Electrical

## License

Copyright (C) 2014 Team 1538 / The Holy Cows

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Architecture

### CowBase

CowBase inherits from IterativeRobot.  Its duties are to handle auto mode selection and connect the robot (CowRobot) to a controller (GenericController).

### CowRobot

CowRobot represents the physical robot in software.  It receives input from a controller and coordinates the physical subsystems of the robot to execute the controller's commands.

Example: Neither the Winch nor the Intake systems are aware of each other.  When the controller tells the robot to fire, CowRobot independently coordinates the Winch and Intakes to fire the catapult.

### Controllers

Controllers generate data to pass to CowRobot.  OperatorController reads from the CowControlBoard (a representation of the physical driver station), performs some logic checks (detects rising edges on push switches, etc), and tells the robot how to behave.  AutonomousController works on the same principles, but reads from a list of autonomous commands instead of a control board.

### CowLib

#### CowGyro

CowGyro is a custom gyro class for IterativeRobots.  It requires that you repeatedly call ```HandleCalibration```, which saves calibration data to an n-element ring buffer every ```COWGYRO_ACCUMULATION_PERIOD``` seconds where n is ```COWGYRO_RING_SIZE```.  Calling ```FinalizeCalibration``` will sum the data in the ring buffer and put it into use.

#### CowLexer

CowLexer is a simple tokenizer utility.  To use it, create a CowLexer object and add token patterns (regular experssions), ex. ```lexer->AddToken("Word", "[A-Za-z]+");```.  Tokens with no name are ignored.  Calling ```TokenizeString``` will return a vector of st_Token structs which have type and value fields.  A simple grammar for parsing INI files can be found in CowConstants.cpp.
