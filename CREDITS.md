DotCam :
-Project creator
-Ocean Manager
-Ocean Shader
-Ocean material

Handkor :
-Initial Shader
-Code for Buoyancy


TK-Master :
-BuoyancyComponent
-BuoyancyForceComponent
-BuoyantDestrutible
-Infinite Ocean System
-Screen Space Reflections (SSR)
-Planar Reflection support
-Distance Blended Normals
-Configurable Sets of Panning Normals (Small, Medium, Far)
-Heightmap-based Seafoam & Foam Wave Caps (Thanks Tin Le!)
-Cubmap-based Reflections (optional)
-Exposed many parameters to BP_Ocean. - You can now easily switch between shader versions, change textures, normals, foam, cubemap, heightmap etc.
-Underwater Distortion Effect
-Underwater Caustics

EvoPulseGaming :
- Current project owner/manager
- IWYU compliance fixes
- Conversion into the Environment Plugin Project (Seperation of Ocean Project into individual plugins, cleanup, de-bloating)
- Physically correct sun/moon position code.
- Several fixes and improvements to the sky system (eclipses, clouds hiding sun/moon, sun/moon/clouds hiding stars)

NilsonLima :
- Various tutorials

Nadrugal :
-Additions to the SkyDome


Burnrate (Justin Beales) :
-AdvancedBuoyancyComponent
* Created with Misc. Games and Intelligent Procedure for:
* Fishing: Barents Sea
* http://miscgames.no/
* http://www.intelligentprocedure.com/ 

QuantumV :
-BuoyantMesh

Komodoman :
-Fish flocking AI and a variety of fish:



Project maintainers (past and current):
- DotCam
- Zoc
- NilsonLima
​​​​​​​- EvoPulseGaming
- TK-Master



BlueEagle :
- Tons of code/project cleanup, conversion of ocean to cpp

Przemek2122 :
-  Refactored Buoyancy Component

Beskar_Mando :
- New buoyancy system (In progress)

Snarf :
- Fixed coding conventions on Fish System, fixed GetDayOfYear crash, moved underwaterbp to c++ for 2ms speed up (not imported yet)

Mordynak:
- World Composition dev

BlueEagle (ajfurey) :
- Converted Ocean BP Variables to c++, and project clean up
----------------------------------------------------------------

This system uses techniques from many tutorials, however it would not have been possible without the help of the following users (Especially Handkor, he deserves a lot of credit for this!):
Handkor - https://forums.unrealengine.com/community/work-in-progress/6668-dynamic-physical-ocean?7440-Dynamic-physical-ocean=
JBaldwin - https://forums.unrealengine.com/community/work-in-progress/2938-jbaldwin-ue4-content-preview-thread?2793-JBaldwin-UE4-Content-Preview-Thread=
berna - https://forums.unrealengine.com/community/work-in-progress/12418-parametric-beaufort-scale-ocean?26317-Parametric-quot-Beaufort-scale-quot-ocean=
ufna - https://forums.unrealengine.com/development-discussion/engine-source-github/4358-plugin-ocean-surface-simulation-plugin-vaocean?4181-Plugin-Ocean-surface-simulation-plugin-(VaOcean)=&highlight=vaocean
Ehamloptiran - https://forums.unrealengine.com/development-discussion/engine-source-github/3775-project-fluidsurface-plugin?3609-Project-FluidSurface-Plugin=
KhenaB - (dead link) https://forums.unrealengine.com/community/work-in-progress/18725-subside-dev-progress?47316-Ocean-Storm-WIP=&highlight=ocean+storm
gregdumb - https://forums.unrealengine.com/community/community-content-tools-and-tutorials/1735-tutorial-quick-time-of-day-setup?1613-Tutorial-Quick-Time-of-Day-Setup=&highlight=gregdumb
and many others.

For those wanting more info on how the waves are created mathematically, THIS ARTICLE goes over everything. Specifically have a look at the section on "Gerstner Waves" for the actual equations that produce the waves.
(dead link) http://developer.download.nvidia.com/GPUGems/gpugems_ch01.html
(new link) https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects
----------------------------------------------------------------