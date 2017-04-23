# Fog Of War concept

Fog Of War concept refers to enemy units, and often terrain, being hidden from the player; this is lifted once the area is explored, but the information is often fully or partially re-hidden whenever the player does not have a unit in that area: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Fogofwar_parts.jpg?raw=true)

At that, we can clearly distinguish between 3 different areas in every FOW

* **Clear**: (Reveled area in the picture) This is the area that the player is able to see, normally it's the space surrounding him. They can be cases (Age Of Empires) were clear area is also the area surrounding buildings of your property. 
* **Dim**: (FogOfWar area in the picture) This is the are that the player has visited at least one time, so it has knowledge about it.
* **Dark**: (BlackMask area in the picture) This is the area where the player has never been, so it draws completely black. 

### How it affects the main gameplay? 

In strategy games, certain planning is needed and the surprise factor plays against you, it would be as easy as boring to play League Of Legends knowing where every player is on the map, that means knowing everything about the enemy (where they go, who is he/she probably attacking next, what character are they...), if it happens players will just increase the weapons backup and damage, it would end to a mechanical and monotone gameplay. So, as we see, fog of war not just change the tiles that you are not close to, it also hides all the enemies, buildings, items etc. that the player could find.

That is the main reason why fog of war exists, players must be ready for facing any character the best way they can at any moment, that works together with help from other players and these are strategic games! 

### Are there different types of FOW?

As the core of FOW is to limit the visibility, obviously yes, there are a lot of ways to cut the field of view of a player. Let's say that the designer just wants the black area to be displayed, without any Dim area, it will be also fog of war, or even if he/she just wants Dim area, it will also be keeping the fog of war essence. The main difference is how the programmer can implement FOW but this will be seen on the following lines. To sum up, everything limiting the player's visibility that is directly affecting to the gameplay is considered fog of war. 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/fog_of_war-2.jpg?raw=true)

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/steamworkshop_webupload_previewfile_332927191_preview.jpg?raw=true)

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/amnesia-the-dark-descent-pc-023.gif.jpg?raw=true)

# A little bit of history

## Origins

The word expression Fog Of War comes from the limitation of the field of view in the military conflict, the word "fog" expresses uncertainty in war and was introduced first by the Prussian military analyst Carl von Clausewitz in his posthumously published book, Vom Kriege (1832). 

## First games

At the beginning of the video game industry, developers thought about capabilities that they could not afford before, hiding information from the player is one of them, the first registered game that actually used the Fog Of War was **_Empire_** by _Walter Bright_. Another example is **_Tanktics_** by _Chris Crawford_. But this las one was criticized for its unreliable and "confusing" Fog Of War system.

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Empire.jpg?raw=true)

Empire: 
https://www.youtube.com/watch?v=sH8eqPnSSEo

After some time, an article wrote by Dave Arneson revealed that Fog Of War was "one of the biggest 'plus' factors in computer simulations", he used _Tanktics_ as an example, Chris Crawford also affirmed that games become less fun as the Fog Of War is more realistic. 

From this moment Fow Of War will start to appear in most of the strategy games that will be programmed. 

# Fog Of War Methods
 
## Tile-Based FOW

In some cases, games are made using tilesets, that means that the thing drawn will be made up of small squares, each one will have an ID corresponding to the tileset used. 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/F5Lzo.png?raw=true)

For implementing that FOW we just have to keep track of what is happening in every tile at every moment of the game, it's normally made with a 2D container of the same dimensions of the map that will be filled with 0, 1 or 2 depending on if it's clear, dim, or black. 

The only thing to do is to know where players are, what the radius of the FOW is and paint that zone. Then, at the time of drawing each tile, we just have to look on what is the number in the FOW container, then do whatever is needed for the game (hide enemies, draw different tiles...).

It may sound easy until this point but have in mind that this way does not remove jagged edges (not softened ones), so the algorithm will grow on complexity if we want the tiles to be softer, despite all of that it will make a great difference since we do not want that: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/lOunb.png?raw=true)

We want this:

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/E0DP3.png?raw=true)

There are a lot of ways of programming that, once better than others, for example: 

 1. We could draw different pixel colors depending on where is the player and where the black area starts. 
 2. We could also change the color depending on the tile's position.
 3. We could generate a tileset with all borders and corners softened and then apply them when they are needed.  

### Advantages 

* Just one container for the FOW need to be created, regardless of what the number of players is. 
* Adjusting the radius is just changing a parameter, everything will work equally. 
* The programmer knows exactly what tiles are beeing visited without any extra calculations. (good for hiding stuff) 
* Better for lighting effects. 

### Drawbacks

* The jagged edges are not solved.
* Keep track of what is the frontier for every player at every moment. 
* ITERATIONS!  

## Using a PNG

-----------

_This documentation explains theoretically how this method is implemented since it is just an alternative and it is not the one beeing used in the solution archive, the theory is based on the documentation linked below. Visit [this link](http://stackoverflow.com/questions/13654753/sdl-drawing-negative-circles-fog-of-war) to get a more detailed explanation on that method and some code examples._   

-----------

This method consists of using a png with a circle on it, the area of that circle will be the field of view that the FOW has, let's see the process more detailed: 

Before starting with the implementation we must have the map drawn, it does not care how it is drawn (using an image or tilesets). This is how the program should look at the beginning.

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/UhpUh.png?raw=true)

The first thing to do is to draw a big surface overlapping with the map, if we do it correctly we should see the screen with the color of the rectangle you just have drawn and nothing else. After that we should create a 32x32 circle like the one below, it is important to keep in mind the color of the outer part of the circle, it has to be the same than the rectangle overlapping the map. 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Xkxt3.png?raw=true)

The next thing to do is to iterate within the circle surface in order to update the alpha of the surface below, after that, we should have the character displayed inside of the circle, being the background the current map loaded, changing the variable of `KeepFogRemoved` to true will keep the area that the player has visited displayed:

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/FhbYT.png?raw=true)

Some validations could be done for making sure we do not write out of our circle buffer. Once we are here, the only thing lasting is to have the 3 different FOW states (clear, dim and black) implemented. To do this, we have to keep the SDL_Rect of where I have been in the FOW, if the alpha is lower than a certain value we should keep that value. 

For implementing a gradient to the circle we could modify the texture of the 32x32 bit circle and this would be in off, another way to make it is to change the opacity of each pixel depending on the position. This should be the result   

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/jkxS5.png?raw=true)

### Advantages 

* Smooth edges are done, there are actually no edges! 
* Coding it is simpler than the Tile-Based method. 

### Drawbacks

* If we want to apply FOW to bigger objects we should change the texture, that mean having a texture for every size of an entity inside the FOW.

# C++ Implementation

The FOW is implemented following the Tile-Based method previously explained: 

Before starting with the implementation of this method we should have a simple map be drawn using a tileset, in this case, the map is generated using Tiled. If you do not know how to generate a map and print it check [this link](http://www.cplusplus.com/forum/general/18315/) for more information. We should have somewhere in the code a container of `int` with the same dimensions as the map containing the ID of every tile on the map. 

The code will be based on the BFS method at the beginning for finding the tiles that the player can see, being the starting point the player's position (more information on how BFS works here), but we will only keep the frontier for optimization, after that, every time that the player change of tile the frontier will be updated and the container inside of the FOW class will be updated too. 

## Pillars of the implementation

* **Generating the edge texture:** apart of containing the translucid square we will use this for making the edge softened drawing a different section of the image depending on what the number is in the FOW array. This implies that the FOW container will not just be filled with 0, 1 and 2, we will use a number for every tile in the image, I strongly recommend to make an `enum` out of that.  

* **Fog Of War Container:** this is a 2D container inside the FOW class with the same dimensions as the map, for now, it will be filled with 0 if it is a black area, 1 if it is dim area and 2 if the character is currently in that place. At the time of drawing this container will be checked, depending on what number the FOW vector contains different FOW tilesets will be drawn on top.  

* **Fog Of War Unity:** is needed to keep track of some data of the players that are inside of the fog of war, for example, what is its frontier or what is its current position, luckily there is no need to store a pointer to the player entity, keeping track of its position will be enough.

       struct player_unity
	list<iPoint>	frontier 
	iPoint	        player_pos        

* **Algorithm:**: obviously the core algorithm for making the Fog Of War happen. 

Adding container lists to store the entities and some functions to manage them will be enough for having the pillars of the algorithm. Moreover, in this method, there are 2 different basic types of entities created, `player` and `simple_character`, the first one are those that the player can move with the keyboard while the others are considered enemies and will hide if the player is not near.  

# Implementation

--------

_In this page the steps for implementing the FOW will not be detailed explained, the way function works is a personal implementation and can be done in multiple ways,  you can check the [code (https://github.com/rogerta97/FogOfWar/tree/master/Motor2D) for viewing the functionality that I personally implemented._ 
 
--------

## Logic Behind The Algorithm

We start from the simple base code where we have a map drawn by tilesets and a simple entity created. I will use sprites of _Zelda: Link To The Past_. 

NOTE: The only thing that makes the tiles be visible or not is the id of the data of the Fog Of War. When we talk about "set" or "fill" we mean modifying the id in order to be printed in a way or in another. 


**1. Setting frontiers**

The first thing we have to do is to know where is this player inside the map. Once we know that, we should create a function that set some points as a frontier, it needs to be a closed frontier.The points that form part of the frontier and the visible area can be stored in a container and then modify the value of the fog of war data, but they can also modify the data instantly. In this documentation, we will use the shape of a circle but any shape should work too. Summing up, we must look for that: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Frontier_logic.jpg?raw=true)

**2. Filling the frontier**

Once we have the frontier of the player stored somewhere it is time to fill it! This should be the result:

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Fog_of_war_filled_logic.jpg?raw=true)

**3. Make the Update**

This is what the program will have to do every iteration, we should have an id for every part of the Fog Of War tileset, when the player moves we should identify somehow what is the direction that the player is taking and move the current points of the corresponding player. Apart from that, if some point on the map stops beeing inside the current field of view of the player we should draw an opaque texture on top of the tile in order to make it darker, it should be something like that: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Fog_Moved_logic.jpg?raw=true)   

**Dont forget updating the characters visibility**

Hidding terrain is not the only thing that Fog Of War can do, it would be capable of hiding stuff when the player is not near. I would simply recommend to keep a boolean variable related to the player and change it to 'true' or 'false' for knowing if it will have to be printed or not. 

## Code Implementation Exercice

This is an exercise for loosing the fear to implement it in your games:) Mind that this is a base code of the Fog Of War and it will need to be adapted to your game. 

### TODO 1 - Adapt `Draw()` function

Before starting with the implementation we should need to adapt the 'draw()' function if 'j1Map' in order to have in count what is in the Fog of War array, in my case it will not print if the id of the Fog Of War data is 0, if it is 1 it will print an opaque rectangle above, and it will behave like normally if it is 2. 

If we have in count the enum created on FogOfWar.h instead of a 2 it will behave normally with 40, that is because after we will need to soft the edges. 

### TODO 2 --- Get Shapes as Frontiers

We have a player_unit for the fog of war that contains: 

   list<iPoint>   frontier; 

	list<iPoint>	current_points; 

	iPoint	        player_pos; 

	int		id; 

Our objective is to fill all of this data before starting with the core of the Fog Of War, it needs to be set before the system starts to update. the function for creating the player_unit and adding it to the list is already created and 'player_pos' and 'id' are already set, so: Fill the frontier list with points forming a closed frontier. 

In my personal case I made BFS from the player position until a certain radius, once there I just return the frontier and I use this technique for making the rounded shape: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/DeletePicks.png?raw=true)

### TODO 3 --- Fill frontiers 

In this step, we will fill the frontier, think on that when we have the frontier filled we will have all the points that the character is currently visiting. Those will be the points that will need to be moved in case that the character moves too so we can store them in the vector of `current_points` of the `player_unit` structure. I check from every tile of the frontier and I save the tiles on the right until another point of the frontier is found, in this way the algorithm works for every shape that has two points on the same 'y' height (triangles, rectangles, circles...).

-------

If we are doing it correctly in this moment we should have the frontier with the shape you decided to all filled with visible tiles, it should look like this:

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Frontier_fill.png?raw=true)

-------

### TODO 4 --- Move the frontier

Now we will have to move the field of view of the Fog of War, the hard part of it is to actually know what are the tiles that you were visiting in the previous position and you are not visiting now, those tiles will be the ones that need to have an opaque rectangle on top. Remember that you don't just have to move each point of the `current_points` property, you also have to update the Fog Of War Data. 

If you did it well, you should have the Fog Of War already working in your program, with player beeing able to move and the system being able to hide terrains or print them darker. 

### TODO 5 --- Update the players

Now we must focus on other characters in order to print them or not. Make a function `ManageCharacters()` that checks if a simple character is visible or not, then modify the whatever needed for this action beeing able to happen. 

IMPORTANT: The visibility of the characters must not be managed by the FogOfWar class, the optimal way to do that would be just to change the bool variable inside the `simple_player_unity` and let the Entities Manager do the rest of the work. 

----------

You already have Fog Of War working in your program! But we could do some improvements... Go to optional improvements if you want to add soft edges to your system! 

# Optional improvements

## Jaggie Edges 

A jagged edge is an edge that is not soft and rounded, the edge of a pixel or the edge of a tile. Unless the game is not done intentionally with jagged edges it uses to be ugly at the view of the player.

Jagged edges are a common problem in the Tile-Based method since the map is built from squares, I will propose 3 extra tasks in order to implement the algorithm that I thought of. 

### TODO 6 ---- (Optional) Remove Side Edges 

Right now we have the Fog Of War fully implemented, we have the struct `player_unit` that contains the frontier but we just used it at the beginning for filling the inner points of that frontier. If we want to implement this algorithm we should take care about the frontier too when te player is moving, this is because we will check each tile on the frontier and its surroundings. 

The way to do it is iterating through the frontier of the player and checking what are its neighbor tiles, but make it orderly. First, we will check for left and right since they will never overlap. The way of making checkings will be accessing to the Fog Of War data container through `Get()` function:  

    if (Get((*it).x - 1, (*it).y) == dim_middle) && Get((*it).x, (*it).y) == dim_clear)); 

Once we made that checking for left and right we should obtain this: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Soft_Sides.jpg?raw=true)

After that, we should do the same iteration but now we will check for the verticals, here collisions start to appear: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Soft_vertical.jpg?raw=true) 

For solving it, we should draw the corresponding corners depending on the way they intercept, in the previous case, for example, a left tile + an up tile, it forms an inner corner on the upper-left side. 

NOTE: We should not iterate again, we should check what is in the tile before setting another id!

This should be the result:  

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Softened_inner_coners_done.jpg?raw=true)

The only thing lasting are those little corners on the inner part of the frontier. I would suggest checking further every time you find a corner! This should be the final result: 

![](https://github.com/rogerta97/FogOfWar/blob/master/Wiki%20images/Jagged_finished.jpg?raw=true)

### TODO 7 --- (Optional) Remove the dark jagged edges

The way of proceeding is the same as the previous one, the only extra think that we should take in account is whether the tile that you are fading in is from the mid area or from the clear area, in the first case you should print the map, then an opaque layer, and then the black gradient, while in the second case you print the black gradient directly after printing the map.   

