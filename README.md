# Project Description: Orc Slayer

Orc Slayer is a simple yet immersive video game prototype in which the player assumes the role of an archer whose objective is to eliminate hordes of orcs in a battlefield. 

The battlefield is set in a modified night-time environment, featuring height maps and blend maps to create a more immersive environment than a flat grassy field. It includes different models of varying sizes, such as a castle, an archery range, a cave, and a campfire.

In our battlefield setting, orcs approach the archer from random directions, and the player must shoot them down with their bow and arrow. While orcs die with a single shot, an infinite number of them will continue to arrive, increasing a counter that tracks the player's defeated opponents.

The player's only weapon is their bow and arrow, and they can only shoot one arrow at a time. The arrow can be reused once it hits the ground or kills an enemy.

Orc Slayer offers an engaging and fast-paced gameplay experience that will challenge the player's skills and reflexes.

---

## Game Usage

### Controls

### Controller
#### Move Player 
<a href="https://drive.google.com/uc?export=view&id=1awyunmj5vol5m5qQeKqSvyQ6RNzJfd6N"><img src="https://drive.google.com/uc?export=view&id=1awyunmj5vol5m5qQeKqSvyQ6RNzJfd6N" style="max-width: 60%; height: auto" title="Click to enlarge picture" />
Action | Input
--- | ---
Move Forward | `Left-Joystick up`
Move Backwards | `Left-Joystick down`
Strafe Left | `Left-Joystick left`
Strafe Right | `Left-Joystick right`
#### Camera Movement
<a href="https://drive.google.com/uc?export=view&id=1V4Mj4NHPfct4WT6bwWJ8E_mpPRbTMXLk"><img src="https://drive.google.com/uc?export=view&id=1V4Mj4NHPfct4WT6bwWJ8E_mpPRbTMXLk" style="max-width: 100%; height: auto" title="Click to enlarge picture" />
Action | Input
--- | ---
Turn Left | `Right-Joystick left`
Turn Right | `Right-Joystick right`
Turn Upwards | `Right-Joystick up`
Turn Downwards | `Right-Joystick down`
#### Interact
Action | Input
--- | ---
Shoot | `Right trigger` or `B button`
Somersault | `X button`
Jump | `A button`
Punch | `B button`
---
### Keyboard
#### Move Player 
Action | Key
--- | ---
Move Forward | `Keyboard up arrow`
Move Backwards | `Keyboard down arrow`
#### Camera Movement
Action | Key
--- | ---
Turn Left | `Keyboard left arrow`
Turn Right | `Keyboard right arrow`
Turn Horizontal | `Left mouse click + horizontal movement`
Turn Vertical | `Right mouse click + vertical movement`
#### Interact
Action | Key
--- | ---
Shoot | `R`
---
### World
<a href="https://drive.google.com/uc?export=view&id=1wawxaaBoekOcWdhqu3Wu9S7XZVg_xXID"><img src="https://drive.google.com/uc?export=view&id=1wawxaaBoekOcWdhqu3Wu9S7XZVg_xXID" style="max-width: 100%; height: auto" title="Click to enlarge picture" />
- The orcs walk towards the player automatically, whether they're in the players fov or not
- Pressing `R` or `B button` will draw the bow and releasing will shoot, which leads to only being able to use one arrow at a time
- A second arrow can't be shot until the first has collided either with an orc or the floor
- World borders are non existant so the player can fall to an invisible ground outside of the map

## Techincal description

The different functions to be performed are:
- GLFW for handling the window and input controls (mouse, keyboard, controller, screen movement, resizing, etc.)
- GLUT for rendering 3D models on a 2D screen and handling of shaders through OpenGL
- GLM manipulation of matrices and vector multiplication and addition for movements in the scene as well as object hierarchy
- ASSIMP to import 3D models
- ALUT for import and playback of sounds in 3D spaces
- CGALib for resolution of shaders, bones, shadows, models, colliders, and other various tools of common use but little manipulation beyond their initial implementation (i.e. time manager within the game).
- The first thing we needed to achieve was the rendering of a flat field on which we correctly graphed our archer.
- We gave the plane a terrain on which the archer would move, a height, a texture, and dimensions large enough for our archer to move in it.
- We created a skybox to have an ambiance and added a light that would function as sunlight. (The shadow was implemented much later).
- We added one or two additional models just to demonstrate that the functioning of the 3D field was correct and that handling the models would not make (for example) the archer's jump make the other models in the field jump as well.
- We manipulated the way we referred to the models and their mat4 matrix a lot. Finally, we opted to use an array system with pointers to refer to the matrices of each element as well as their movement vectors, their indices for each animation, whether they are alive or not, etc.
- We opted to handle much of the animations, movements, the birth of different models, etc. in methods different from those initially established (loop and init) thus giving us a little more space and freedom for the manipulation of the variables in exchange for having a few more global variables than we had in the other way.
- We added the use of shadows and fog in the distance in a similar way to how we had done it in previous practices.
- The 3D models and textures we used were mostly obtained from Synty, a website that sells models for video game development. However, one of our team members had previously purchased a bundle of models at a great discount on Humble Bundle, so we had enough models created from the same package while their price was close to zero.
- Manipulating the models was not as simple as we had anticipated since, for different reasons, importing FBX files was not as evident and straightforward as we had been able to do in class with example models. Among the steps involved were reconfiguring the pose to the base pose in Blender, recalibrating the sizes, animating the models (in our case with Mixamo), downloading the file in DAE, putting it back in Blender, rescaling, and finally exporting the FBX, always checking that there were no missing parts or textures. This was one of the most challenging parts to get right.
- Among the additional models was the bonfire, which we were able to animate by adding a point light effect that illuminates the surrounding area as well as a particle effect that moves the flames.
- We included more orc models and gave them a very minimal artificial intelligence through which they could follow the game's protagonist, the archer.
- We gave the archer the ability to shoot one arrow at a time. This arrow, when it falls to the ground or hits an enemy, can be thrown again. This works as if the archer had to wait a short reloading time. The arrow can only be aimed with the archer's turn; the falling parabola and firing angle are always the same due to lack of time for better implementation.
