Visit the #faq channel in discord for fixes with known issues:
https://discord.gg/Xzrbpup

10/19/2019
Sascha Elble:
This is the final version (apart from maintenance and bug fixing) for the old version of the ocean project.

4.22/4.23 compatible (with compiling on 4.23)

4.23 special note: A user reported you may/will have to go to plugins and package the ocean project manually or "apex destruction" error will appear, you could also remove apex destruction and turn off "Apex cloth" in the sails of the ships.

Moving forward:
I am starting to re-vamp the project into a new direction called the "Environment" project since this project started heading in that direction. The project will be split up into separate plugins. For now I've started work in the dif-tow branch, this will eventually all be moved to a new GitHub organization. 



Also, for hiding the ocean in certain parts so your (below ocean level) beautiful city does not "flood" look at the masking example map. 


# OceanProject
An Ocean Simulation project for Unreal Engine 4.

---------------------

**MASTER Branch - This is the development branch. Please use this branch for all Pull Requests.**
**MASTER_FULL Branch - This is the "final" version of this project as of 4.22, before it was stripped down

**PLEASE NOTE**

**This branch is NOT guaranteed to properly compile or be stable at all times. This branch will be updated between major UE4 releases with the latest changes, and periodically will be compiled out to a new branch for each new UE4 release, and when major changes occur.**

**Please use the branch associatd with your UE4 engine version for the best compatability.**

---------------------

![img_project](ProjectScreenshot.jpg)

**Changelog:**
Changes in 4.22:
Apart from all the other commits, this was future proofed (I hope) to 4.23 (i.e all warning were fixed).

Changes in 4.18:
* Minor fixes

Changes in 4.17:
* Many bugfixes
* Code cleanup

Added in 4.15:
* Advanced Buoyancy Component by Burnrate.
* Compatibility fixes.

Added in 4.12:
* Implemented Planar Reflections as a component of the BP_Ocean blueprint
* Implemented highly accurate calculations for sun/moon in the plugin, updated skydome to use the new calendar and sky system. This sets the base for the new Skydome, features coming in subsequent updates.

Added in 4.10:

* Underwater post process with exponential fog
* Stencil-based underwater masking
* Wet lens post process
* Landscape heightmap-based displacement modulation
* OceanStorm example map (40 Gerstner waves)
* Advanced BuoyantMesh component by Nubtron
* Accurate height readback by accounting for x,y displacement
* Harpoon gun with rope physics
* Various code improvements
* Various shader improvements  
[(Forum post with more info)](https://forums.unrealengine.com/showthread.php?42092-Community-Project-WIP-Weather-amp-Ocean-Water-Shader&p=485675&viewfull=1#post485675)

Added in 4.8:

* Screen Space Reflections (SSR)
* Distance Blended Normals
* Infinite Ocean System
* Configurable Sets of Panning Normals (Small, Medium, Far)
* Heightmap-based Seafoam & Foam Wave Caps
* Cubmap-based Reflections
* Exposed many parameters to BP_Ocean
* Underwater Distortion Effect
* Underwater Caustics  
[(Forum post with more info)](https://forums.unrealengine.com/showthread.php?42092&p=311982&viewfull=1#post311982)

Added in 4.7:

* Removed the old buoyancy movement component.
* Added new buoyancy force component.
* Added custom player controller.
* Added example assets: wooden barrels, crate, cannon, icebergs, flag/sails (with apex cloth) & warship HMS Victory.
* Added simple UI (UMG), with health bar, ship speedometer (knots) etc.
* Added simple drivable boat blueprint with (multiplayer support).
* Added advanced warship blueprint with physics + per-poly collision
* Updated OceanExampleMap_01 to include example boats, ships, cannons, buoyant objects & destructible icebergs.  
[(Full 4.7 changelist)](https://forums.unrealengine.com/showthread.php?42092-Community-Project-WIP-Weather-amp-Ocean-Water-Shader&p=279737&viewfull=1#post279737)

===================================================================

Discord channel:
https://discord.gg/ewadNBG

(DotCam is MIA, no access, don't use)
Vote for upcoming features on our [Trello](https://trello.com/b/7dzOdkvw/ue4-ocean-weather-project)

(This was re-routed, please see last post on this thread)
UE4 Forums thread with more info: [[Community Project] WIP Weather & Ocean Water Shader](https://forums.unrealengine.com/showthread.php?42092-Community-Project-WIP-Weather-amp-Ocean-Water-Shader)

(Again, DotCam is MIA, contact one of the other project admins)
If you would like to contribute to this project, please PM me on the UE4 forums with your GitHub user name and contact info.
