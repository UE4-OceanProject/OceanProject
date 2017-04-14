# OceanProject
An Ocean Simulation project for Unreal Engine 4.

---------------------

**4.15 Branch - This is the most up to date branch compatible with UE 4.15.x**

![img_project](ProjectScreenshot.jpg)

**Changelog:**

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


Vote for upcoming features on our [Trello](https://trello.com/b/7dzOdkvw/ue4-ocean-weather-project)

UE4 Forums thread with more info: [[Community Project] WIP Weather & Ocean Water Shader](https://forums.unrealengine.com/showthread.php?42092-Community-Project-WIP-Weather-amp-Ocean-Water-Shader)


If you would like to contribute to this project, please PM me on the UE4 forums with your GitHub user name and contact info.
