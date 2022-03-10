# Quantum Quarrel

## Build instructions
- While in root folder of the github, run
> `cocos new quantumQuarrel -l cpp -p com.vgProg2.quantumQuarrel`<br />
> `cd .\quantumQuarrel\proj.win32\`<br />
> `cmake .. -G "Visual Studio 15 2017" -AWin32`<br />
- then open the solution file in visual studio
- run the debugger
- an error should pop up, stating 'Unable to start program... cannot find the file specified.'
- close the popup
- right click on the quantumQuarrel project in solution explorer
- select 'Set as StartUp Project'
- run the debugger again
- the default project should now run 

## Pull instructions
- Pull from github normally
- copy the files inside the 'classes' folder at the root of the github directory
- paste them into the solution explorer classes folder located inside of Source Files
- copy the files inside the 'resources' folder at the root of the github directory
- paste them into the solution explorer resources folder

## Push instructions
- copy the files inside the solution explorer classes folder located inside of Source Files
- paste them into the 'classes' folder at the root of the github directory
- copy the files inside the solution explorer resources folder
- paste the files inside the 'resources' folder at the root of the github directory
- continue to push as normal