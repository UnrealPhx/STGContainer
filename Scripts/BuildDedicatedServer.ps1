$script_path = split-path -parent $MyInvocation.MyCommand.Definition
$engine_path = (Get-ItemProperty -Path 'HKLM:\SOFTWARE\EpicGames\Unreal Engine\4.21' -Name InstalledDirectory).InstalledDirectory
$uat = $engine_path + '\Engine\Build\BatchFiles\RunUAT.bat'
$project = $script_path + '\..\STGContainer.uproject'

iex "$uat BuildCookRun -project='$project' -platform=Win64 -clientconfig=Development -noclient -server -serverplatform=Linux -serverconfig=Shipping -cook -allmaps -build -stage -pak -archive"
