$script_path = split-path -parent $MyInvocation.MyCommand.Definition
$project = $script_path + '\..\STGContainer.uproject'

$engine_key = (Get-Content $project | Out-String | ConvertFrom-Json).EngineAssociation

$engine_path = $script_path + '\..\Engine'
if ($engine_key -match '\.') {
    $engine_path = (Get-ItemProperty -Path 'HKLM:\SOFTWARE\EpicGames\Unreal Engine\' + $engine_key -Name InstalledDirectory).InstalledDirectory
}
else {
    $engine_path = (Get-ItemProperty -Path 'HKCU:\Software\Epic Games\Unreal Engine\Builds' -Name $engine_key).$engine_key
}

$uat = $engine_path + '\Engine\Build\BatchFiles\RunUAT.bat'

$archive_dir = $script_path + '\..\Saved\ArchivedBuilds'
Invoke-Expression "$uat BuildCookRun -project='$project' -platform=Win64 -clientconfig=Development -noclient -server -serverplatform=Win64 -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory='$archive_dir'"
Invoke-Expression "$uat BuildCookRun -project='$project' -platform=Win64 -clientconfig=Development -noclient -server -serverplatform=Linux -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory='$archive_dir'"

