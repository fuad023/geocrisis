# GeoCrisis
<h6>APOLOGISE: Here I am writing my very first markdown language, so you are requested to view it in code perspective for intended formatting.</h6>  

The whole point of this project is to mimic the game Geometry Wars.  
  
The project "GeoCrisis" has the following features:  
  
Player:
- The player is represented by the red octagon shape which is defined in the config file
- The player spawns in the center of the screen at the beginning of the game, and after it dies (collides with an enemy)
- The player moves by a speed read from the config file in these directions:    
  <b><i>Up: W key, Left: A key, Down: S key, Right: D key  
  Up/Down: Gamepad left stick(Y-axis), Left/Right: Gamepad left stick(X-axis)</i></b>
- The player is confined to move only within the bounds of the window
- The player shoots a bullet towards the mouse pointer when the <b><i>left mouse button is click</i></b>ed, or towards to point <b><i>right stick of the gamepad</i></b> is directed.
- Bullets shot by the player has a certain lifespan in which when is runs out, the bullet fades away.
- The speed, size and lifespan of the bullets are read from the config file.

Special Ability:
- When the <b><i>right mouse button is click</i></b>ed <b><i>(or right bumper in controller)</i></b>, a special attack can be performed by the played which would destroy half of the enemies. But the penalty of it is that you will not secure any points from those destroyed enemies.
- There is a cooldown timer at the bottom-left of the screen to showcase when this attack can be performed.

Enemy(s):
- Enemies spawns in a random location near the outside of the egde of the screen every X frames, where X is defined in the config file.
- The number of vertices, radius, minimum and maximum speed of the enemies is defined in the config file.
- When big enemies gets destroyes, N small enemies spawns in its place, where N is the number of vertices of the original enemy.

Gui:
- Here I have included a debug mode in the bin folder "debug.exe" where many systems of the game can be manipulated.
  Also all the entities in this game can be tracked using this user interface

Misc:
- The <b><i>P key</i></b> in the keyboard and the <b><i>START button</i></b> in the gamepad pauses the game.
- The Esc key</i></b> in the keyboard and the <b><i>BACK button</i></b> in the gamepad closes the game.

Configuration File:

The config file in the bin folder (config.txt) has one line each specifing the specification for window, player and bullet, big and small enemy.
Lines are given in the order, which the following systax

Windows <i>W H FL FS</i>
- Window Width        W             <i>unsigned int</i>
- Window Heigth       H             <i>unsigned int</i>
- Frame Limit         FL            <i>unsigned int</i>
- Fullscreen mode     FS            <i>bool: 1 for fullscreen mode, 0 for windowed mode</i>

Font <i>F S R G B</i>
- Font File           F             <i>std::string</i>
- Font Size           S             <i>int</i>
- RGB Colour          R G B         <i>int int int</i>

Player Specification:
Player <i>SR CR S FR FG FB OR OG OB OT V</i>
- Shape Radius        SR            <i>double</i>
- Collision Radius    CR            <i>double</i>
- Speed               S             <i>double</i>
- Fill Colour         FR FG FB      <i>int int int</i>
- Outline Colour      OR OG OB      <i>int int int</i>
- Outline Thickness   OT            <i>double</i>
- Shape Vertices      V             <i>int</i>

Enemy Specification:
Enemy <i>SR CR SMIN SMAX OR OG OB OT VMIN VMAX L SI</i>
- Shape Radius        SR            <i>double</i>
- Collision Radius    CR            <i>double</i>
- Min/Max Speed       SMIN SMAX     <i>double double</i>
- Outline Colour      OR OG OB      <i>int int int</i>
- Outline Thickness   OT            <i>double</i>
- Min/Max Vertices    VMIN VMAX     <i>int int</i>
- Small Enemy Lifespan L            <i>int</i>
- Spawn Interval      SI            <i>int</i>

Bullet Specification:
Bullet <i>SR CR S FR FG FB OR OG OB OT V L SI</i>
- Shape Radius        SR            <i>double</i>
- Collision Radius    CR            <i>double</i>
- Speed               S             <i>double</i>
- Fill Colour         FR FG FB      <i>int int int</i>
- Outline Colour      OR OG OB      <i>int int int</i>
- Outline Thickness   OT            <i>double</i>
- Shape Vertices      V             <i>int</i>
- Lifespan            L             <i>int</i>
- Spawn Interval      SI            <i>int</i>

Gamepad O
- Operational         O             <i>bool: 1 to operate in gamepad mode, 0 to to operate in keyboard mode.
                                          <b>WARNING: DO NOT USE KEYBOARD WITH GAMEPAD MODE TURNED ON.</b></i>


