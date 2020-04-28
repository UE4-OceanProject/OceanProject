访问#faq频道以获取已知问题的修复程序:
https://discord.gg/Xzrbpup

04/29/2020
Kaisaier:
Compiling on 4.24.3


10/19/2019
Sascha Elble:
这是海洋项目旧版本的最终版本（除了维护和错误修复）。

兼容4.22 / 4.23（在4.23上编译）

4.23特别说明：用户报告您可能/将不得不删去插件并手动打包海洋项目，否则将出现“顶点破坏”错误，您还可以移除顶点破坏并关闭船帆中的“顶点布” 。

未来发展：
我开始将项目重新调整为一个新的方向，称为“环境”项目，因为该项目开始朝这个方向发展。该项目将拆分为单独的插件。现在，我已经开始在dif-wow分支中工作，最终将所有这些移至新的GitHub组织中。



另外，为了将海洋隐藏在某些部分中，以便您（在海平面以下）美丽的城市不会“淹没”在遮罩示例地图上。


# 海洋项目(OceanProject)
虚幻引擎4的海洋仿真项目.

---------------------

** MASTER分支-这是开发分支。请对所有请求请求使用此分支。**
** MASTER_FULL分支-这是该项目从4.22开始的“最终”版本，之前已被剥离

**请注意**

**不保证此分支始终正确编译或稳定。该分支将在UE4的主要发行版之间进行更新，并具有最新更改，并会在出现重大更改时针对每个新的UE4发行版定期编译成新的分支。**

**请使用与您的UE4引擎版本关联的分支以获得最佳兼容性。**

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

Discord 频道:
https://discord.gg/ewadNBG

(DotCam is MIA, no access, don't use)
Vote for upcoming features on our [Trello](https://trello.com/b/7dzOdkvw/ue4-ocean-weather-project)

（这已被重新路由，请参阅此线程上的最新帖子）
UE4论坛中包含更多信息的主题：[[社区项目] WIP天气和海洋水着色器]（https://forums.unrealengine.com/showthread.php?42092-Community-Project-WIP-Weather-amp-Ocean-Water-着色器）
**请使用与您的UE4引擎版本关联的分支以获得最佳兼容性。

（同样，DotCam是MIA，请联系其他项目管理员之一）
如果您想为这个项目做贡献，请在UE4论坛上用我的GitHub用户名和联系信息来与我互动。
