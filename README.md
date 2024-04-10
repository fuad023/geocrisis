# GeoCrisis
The whole point of this project is to mimic the game Geometry Wars.

<p>
The project "GeoCrisis" has the following features:

Player:
- The player is represented by the red octagon shape which is defined in the config file
- The player spawns in the center of the screen at the beginning of the game, and after it dies (collides with an enemy)
- The player moves by a speed read from the config file in these directions:
  Up: W key, Left: A key, Down: S key, Right: D key
  Up/Down: Gamepad left stick(Y-axis), Left/Right: Gamepad left stick(X-axis)
- The player is confined to move only within the bounds of the window
- The player shoots a bullet towards the mouse pointer when the left mouse button is clicked, or towards to point right stick of the gamepad is directed.
- Bullets shot by the player has a certain lifespan in which when is runs out, the bullet fades away.
- The speed, size and lifespan of the bullets are read from the config file.

Special Ability:
- When the right mouse button is clicked, a special attack can be performed by the played which would destroy half of the enemies. But the penalty of it is that you will not secure any points from those destroyed enemies.
- There is a cooldown timer at the bottom-left of the screen to showcase when this attack can be performed.

Enemy(s):
- Enemies spawns in a random location near the outside of the egde of the screen every X frames, where X is defined in the config file.
- The number of vertices, radius, minimum and maximum speed of the enemies is defined in the config file.
- When big enemies gets destroyes, N small enemies spawns in its place, where N is the number of vertices of the original enemy.

Gui:
- Here I have included a debug mode in the bin folder "debug.exe" where many systems of the game can be manipulated.
  Also all the entities in this game can be tracked using this user interface

Misc:
- The P key in the keyboard and the START button in the gamepad pauses the game.
- The Esc key in the keyboard and the BACK in the gamepad closes the game.

Configuration File:

The config file in the bin folder (config.txt) has one line each specifing the specification for window, player and bullet, big and small enemy.
Lines are given in the order, which the following systax

Windows W H FL FS
- Window Width        W             unsigned int
- Window Heigth       H             unsigned int
- Frame Limit         FL            unsigned int  
- Fullscreen mode     FS            bool: 1 for fullscreen mode, 0 for windowed mode

Font F S R G B
- Font File           F             std::string
- Font Size           S             int
- RGB Colour          R G B         int int int      

Player Specification:
Player SR CR S FR FG FB OR OG OB OT V
- Shape Radius        SR            double    
- Collision Radius    CR            double
- Speed               S             double    
- Fill Colour         FR FG FB      int int int        
- Outline Colour      OR OG OB      int int int        
- Outline Thickness   OT            double    
- Shape Vertices      V             int       

Enemy Specification:
Enemy SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI
- Shape Radius        SR            double    
- Collision Radius    CR            double
- Min/Max Speed       SMIN SMAX     double double       
- Outline Colour      OR OG OB      int int int        
- Outline Thickness   OT            double    
- Min/Max Vertices    VMIN VMAX     int int
- Small Enemy Lifespan L            int
- Spawn Interval      SI            int 

Bullet Specification:
Bullet SR CR S FR FG FB OR OG OB OT V L SI
- Shape Radius        SR            double    
- Collision Radius    CR            double
- Speed               S             double    
- Fill Colour         FR FG FB      int int int       
- Outline Colour      OR OG OB      int int int        
- Outline Thickness   OT            double 
- Shape Vertices      V             int   
- Lifespan            L             int
- Spawn Interval      SI            int 

Gamepad O
- Operational         O             bool: 1 to operate in gamepad mode, 0 to to operate in keyboard mode.
                                          WARNING: DO NOT USE KEYBOARD WITH GAMEPAD MODE TURNED ON.

  </p>
