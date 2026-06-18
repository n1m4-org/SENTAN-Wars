[CmdletBinding()]
Param(
    [string]$Destination = ".",
    [string]$Configuration = "Release"
)

if (-Not (Test-Path $Destination)) {
    New-Item -ItemType Directory -Path $Destination | Out-Null
}

Set-Variable -Name "SolutionRoot" -Value (Resolve-Path $PSScriptRoot\..).Path -Option Constant
Set-Variable -Name "GameRoot" -Value (Resolve-Path "$SolutionRoot\app").Path -Option Constant
Set-Variable -Name "EngineRoot" -Value (Resolve-Path "$SolutionRoot\module\TakoEngine\project\engine").Path -Option Constant
Set-Variable -Name "BinPath" -Value (Resolve-Path "$SolutionRoot\app\bin").Path -Option Constant

if (-Not (Test-Path "$BinPath\$Configuration")) {
    Write-Host "Error: Bin directory not found at $BinPath\$Configuration" -ForegroundColor DarkRed
    exit 1
}

Get-ChildItem -Path "$BinPath\$Configuration" | 
Where-Object {$_.Extension -in '.exe', '.dll'} | 
ForEach-Object {
    Write-Host "Copying $($_.Name) to $Destination" -BackgroundColor DarkBlue -ForegroundColor White
    Copy-Item -Path $_.FullName -Destination $Destination -Force
}

Set-Variable -Name "Resources_Game" -Value "resources" -Option Constant
Set-Variable -Name "Resources_Engine" -Value "EngineResources" -Option Constant
Set-Variable -Name "Resources_Engine_Src" -Value "resources" -Option Constant

if (-Not (Test-Path "$Destination\$Resources_Game"))
{
    Write-Host "Creating directory $Resources_Game in $Destination" -BackgroundColor DarkBlue -ForegroundColor White
    New-Item -ItemType Directory -Path "$Destination\$Resources_Game" | Out-Null
}
if (-Not (Test-Path "$Destination\$Resources_Engine"))
{
    Write-Host "Creating directory $Resources_Engine in $Destination" -BackgroundColor DarkBlue -ForegroundColor White
    New-Item -ItemType Directory -Path "$Destination\$Resources_Engine" | Out-Null
}

Write-Host "Copying resources to $Destination" -BackgroundColor DarkBlue -ForegroundColor White
Write-Host " - Copying $Resources_Game" -BackgroundColor DarkBlue -ForegroundColor White
Copy-Item -Path "$GameRoot\$Resources_Game\*" -Destination "$Destination\$Resources_Game" -Recurse -Force
Write-Host " - Copying $Resources_Engine" -BackgroundColor DarkBlue -ForegroundColor White
Copy-Item -Path "$EngineRoot\$Resources_Engine_Src\*" -Destination "$Destination\$Resources_Engine" -Recurse -Force
