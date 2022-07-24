# Flappy Bird

# 1. Introduction
Flappy Bird is a 2-D game where a player should avoid hitting walls through a tiny hole to finish each level. The player controls their character through a joystick by moving it up and down. The player has four lives; each time they hit a wall, they lose one. Upon finishing all three levels, they win the game. The game has three levels of difficulties of choice prompt before the player chooses to play.

# 2. Hardware Components
• Computing<br />
&emsp; – Elegoo UNO R3 microcontroller<br />
&emsp; – Shift Register<br />
• Inputs<br />
&emsp; – Joystick (Controls character in game) <br />
&emsp; – Reset button<br />
• Outputs<br />
&emsp; – Nokia 5110 LCD screen (display the game).<br />
&emsp; – 1 Digit 7-Segment Display 1PC (display lives remaining for player). <br />
&emsp; – Buzzer for music<br />
&emsp; – LED Lights<br />
&emsp; – Breadboard 1PC<br />

# 3. Basic Functionality
The game consists of a single player. The joystick controls the player’s character (circle or point) to move up and down (vertically) on the screen. The computer will generate ”Walls” and pass them towards the player character; the player must avoid them by positioning their character in line with wall cut-offs ”Holes.”<br />
If the player hits the ”Wall,” they lose a life (one out of four max); the level will start over if they reach zero. The number of lives remaining will be displayed and updated on a 7-Segment Display separate from the game (Nokia 5110 LCD screen).<br />
The player will have the ability to see each level duration time, displayed on 4 Digit 7-Segment Display in seconds. When the player finishes all their level with ≥ 0 lives, the player wins, lights will flash and music will be play (10sec); the game goes back to the start menu. The start menu will have a start option and difficulty level. The game resets when the player presses the reset button.

# 4. User Guide
• Nokia 5110 LCD screen output the whole game including drawing game process, different menus and screens<br />
• The joystick y-axis controls the player’s character (Bird) to move up and down (vertically) on the screen. It also switches between different menu screens. The joystick button click registers the player’s choice.<br />
• Player remaining lives gets displayed on 1 Digit 7-Segment Display. <br />
• Buzzer outputs different songs patterns.<br />
• Green LED flashes indicating the player passed a wall.<br />
• Red LED flashes indicating the player hit a wall.<br />
• Reset button resetting the game upon clicking, limited to during gameplay or while the system is in sleep mode.<br />

# 5. Software Libraries Used
All three libraries below sole purpose are to drive my Nokia 5110 LCD Screen .<br />
• SPI Library allowed me to communicate with SPI devices, in my case Nokia 5110 LCD Screen, with the Arduino as the controller device.<br />
&emsp; https://www.arduino.cc/reference/en/language/functions/communication/spi/<br />
• Adafruit GFX Library is a ’core’ library used for all graphics/text displayed on my screen. The library I have cannot function without this one.<br />
&emsp; https://www.arduino.cc/reference/en/libraries/adafruit-gfx-library/<br />
• Adafruit PCD8544 Nokia 5110 LCD library is a Arduino driver for PC8544, used to operate my screen.<br />
&emsp; https://www.arduino.cc/reference/en/libraries/adafruit-pcd8544-nokia-5110-lcd-library/.

# 6. Wiring Diagram
<img src="WiringDiagram.jpeg?raw=true" width="1000">
