# Start the bat file
start-process -wait -FilePath "run.bat"

# Run the hotreload executable in a new split pane
wt -p "Windows PowerShell" split-pane -H -p "Windows PowerShell" -d . "build\hotreload.exe"

# Run the filewatcher in another new split pane
wt -p "Windows PowerShell" split-pane -p "Windows PowerShell" -d . "build\filewatcher.exe"


