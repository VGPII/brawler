# Installing and Linking FMOD Instructions
  - Download the FMOD library (FMOD Engine, not FMOD Studio) for windows (not Windows UWP), I did so [here](https://www.fmod.com/download#fmodengine)
  - When instantiating a new project with cmake, you will need to copy and paste the contents of the FMOD_LIB folder on the same 
  level as the solution file, or it won’t find the necessary DLL files.
  - Once inside the project in visual studio, right click the quantumquarrel project in solution explorer, and click properties.
  - Inside of properties set the configuration at the top to all configurations, then navigate to the “VC++ Directories” tab 
  inside of configuration properties in the top left.
  - Once inside VC++ Directories add this text to the start of the include directories line:  $(SolutionDir)FMOD_inc;
  - Then, add this text to the start of the library directories line:  $(SolutionDir)FMOD_lib;
  - Now set the configuration to debug.  Navigate to the input tab under the linker section.  Add this text to the additional 
  dependencies line:  fmodL_vc.lib;
  - Now set the configuration to release.  Add this text to the additional dependencies line:  fmod_vc.lib;
  - Your FMOD and project should now be linked, you can check to make sure the FMOD library is being found in the 
  top of AudioManager.h, MainMenu.h, and Player.h where the inclusion of fmod.hpp is.
