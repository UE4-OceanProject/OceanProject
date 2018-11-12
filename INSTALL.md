## Install Instructions

### 4.20



* Download the entire project from github via git or zip download.
* If there is a `ComputeShaderDev` folder in `Plugins` folder, remove it (otherwise it will most likely not work)
* Open `OceanProject.uproject` with **UE 4.20** (It's only tested on **4.20.3**).

* The project should load just fine.

* Follow [this guide from the wiki](https://github.com/UE4-OceanProject/OceanProject/wiki/How-to-add-to-your-project) to install into your own project.


#### How to package (and include OceanPlugin in the build)

* Close your project and UE4

* Move the `OceanPlugin` folder (which is on `Plugins` folder) to `Program Files/Epic Games/UE_4.20/Engine/Plugins/Runtime`

* Edit your `.uproject` and add this:

  ```json
  "Plugins": [
      {
          "Name": "OceanPlugin",
          "Type": "Runtime",
          "LoadingPhase": "PreDefault",
          "Enabled": true
      }
  ]
  ```

  So your `.uproject` may look like this

  ```json
  {
  	"FileVersion": 3,
  	"EngineAssociation": "4.20",
  	"Category": "",
  	"Description": "",
  	"Plugins": [
  		{
  			"Name": "OceanPlugin",
  			"Type": "Runtime",
  			"LoadingPhase": "PreDefault",
  			"Enabled": true
  		}
  	]
  }
  ```


* Open `OceanProject.uproject` with UE 4.20 again.
* Now it should work as before, and also the plugin will be included in the packaged build, so your built game will not crash on startup.