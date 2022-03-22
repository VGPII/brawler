# Quantum Quarrel

## Build instructions
- While in root folder of the github
- delete the 'quantumQarrel/' folder
- run:
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
- close the running project
- run `git restore .` to pull the source files back
- delete the files in the 'classes' folder in the solution explorer
- drag and drop the files in the source folders to the solution explorer
- repeate the last two steps for the resources folder
- build again and you should be caught up

[Displaying Tile Map]()