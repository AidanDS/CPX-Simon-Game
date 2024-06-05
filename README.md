# CPX-Simon-Game
Final Project for ENGR 103; interactive game based on the Simon handheld memory game

## Game Description

The game is a memory game based on a random pattern. The device will generate a random pattern of 1s and 0s which will represent left and right. The device will play the pattern with lights on either the left or right side of the board at a time, with a corresponding sound.

The player has to remember the order of the displayed pattern and enter in a matching pattern using the buttons. If they are correct, the game will confirm their success with a short melody and a green light pattern. If they enter any part wrong, or take too long, the game will tell them with a different sound and display of red lights. 

It will automatically restart after a failure. It can also be restarted at any time by using the switch (not pause anymore because it doesn’t make sense with this game). The player’s score will be displayed on the serial monitor at the end of the game and will be equal to the number of levels passed.

## Inputs and Outputs

Left and right buttons: Will be used by the player to enter the pattern in a certain order.

Switch: Will be used to start/restart the game.

LEDs: Will be used to play the random pattern for each level, confirming inputs from the player, and playing a pattern when the game ends due to a failure.

Speaker: This will be used to indicate the end of each pattern being played/entered. It will also tell the player if they got the answer correct or incorrect.
