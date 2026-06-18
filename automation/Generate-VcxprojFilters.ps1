Param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$VcxprojPath,
    [string]$OutputPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Normalize-IncludePath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$IncludePath,
        [Parameter(Mandatory = $true)]
        [string]$ProjectDirectory
    )

    $normalized = $IncludePath.Trim()
    if ([string]::IsNullOrWhiteSpace($normalized)) {
        return $null
    }

    $normalized = $normalized -replace '/', '\'

    foreach ($macro in @('$(ProjectDir)', '$(SolutionDir)')) {
        if ($normalized.StartsWith($macro, [System.StringComparison]::OrdinalIgnoreCase)) {
            $normalized = $normalized.Substring($macro.Length).TrimStart('\')
            break
        }
    }

    while ($normalized.StartsWith(".\", [System.StringComparison]::OrdinalIgnoreCase)) {
        $normalized = $normalized.Substring(2)
    }

    if ([System.IO.Path]::IsPathRooted($normalized)) {
        $projectPrefix = $ProjectDirectory.TrimEnd('\')
        if ($normalized.StartsWith($projectPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
            $normalized = $normalized.Substring($projectPrefix.Length).TrimStart('\')
        }
    }

    return $normalized
}

function Get-FilterPath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$IncludePath,
        [Parameter(Mandatory = $true)]
        [string]$ProjectDirectory
    )

    $normalized = Normalize-IncludePath -IncludePath $IncludePath -ProjectDirectory $ProjectDirectory
    if (-not $normalized) {
        return $null
    }

    $fileName = [System.IO.Path]::GetFileName($normalized)
    if ($fileName -ieq "main.cpp") {
        return $null
    }

    $directory = [System.IO.Path]::GetDirectoryName($normalized)
    if ([string]::IsNullOrWhiteSpace($directory)) {
        return $null
    }

    if ($directory -ieq "src") {
        return $null
    }

    if ($directory.StartsWith("src\", [System.StringComparison]::OrdinalIgnoreCase)) {
        $directory = $directory.Substring(4)
    }

    if ([string]::IsNullOrWhiteSpace($directory)) {
        return $null
    }

    return $directory
}

function Add-FilterWithParents {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilterPath,
        [Parameter(Mandatory = $true)]
        [AllowEmptyCollection()]
        [System.Collections.Generic.HashSet[string]]$FilterSet
    )

    $parts = $FilterPath -split '\\'
    for ($i = 0; $i -lt $parts.Count; $i++) {
        $path = ($parts[0..$i] -join '\')
        $null = $FilterSet.Add($path)
    }
}

$resolvedVcxprojPath = (Resolve-Path -LiteralPath $VcxprojPath).Path
$projectDirectory = Split-Path -Parent $resolvedVcxprojPath
if (-not $OutputPath) {
    $OutputPath = "$resolvedVcxprojPath.filters"
}

[xml]$projectXml = Get-Content -LiteralPath $resolvedVcxprojPath -Raw
$namespaceUri = "http://schemas.microsoft.com/developer/msbuild/2003"
$namespaceManager = New-Object System.Xml.XmlNamespaceManager($projectXml.NameTable)
$namespaceManager.AddNamespace("msb", $namespaceUri)

$existingFilterIds = @{}
if (Test-Path -LiteralPath $OutputPath) {
    [xml]$existingFiltersXml = Get-Content -LiteralPath $OutputPath -Raw
    $existingFilters = $existingFiltersXml.SelectNodes("/msb:Project/msb:ItemGroup/msb:Filter[@Include]", $namespaceManager)
    foreach ($filter in $existingFilters) {
        $filterName = $filter.GetAttribute("Include")
        $uniqueIdNode = $filter.SelectSingleNode("msb:UniqueIdentifier", $namespaceManager)
        if ($uniqueIdNode) {
            $existingFilterIds[$filterName] = $uniqueIdNode.InnerText
        }
    }
}

$excludedNames = @("ProjectConfiguration", "ProjectReference")
$projectItems = $projectXml.SelectNodes("/msb:Project/msb:ItemGroup/*[@Include]", $namespaceManager) |
    Where-Object {
        $excludedNames -notcontains $_.Name -and
        $_.GetAttribute("Include") -notmatch "\|"
    }

$items = @()
$typeOrder = @()
$filters = New-Object System.Collections.Generic.HashSet[string]

foreach ($item in $projectItems) {
    $include = $item.GetAttribute("Include")
    $filterPath = Get-FilterPath -IncludePath $include -ProjectDirectory $projectDirectory

    if ($filterPath) {
        Add-FilterWithParents -FilterPath $filterPath -FilterSet $filters
    }

    if ($typeOrder -notcontains $item.Name) {
        $typeOrder += $item.Name
    }

    $items += [PSCustomObject]@{
        Type = $item.Name
        Include = $include
        Filter = $filterPath
    }
}

$orderedTypes = @()
if ($typeOrder -contains "ClCompile") {
    $orderedTypes += "ClCompile"
}
$orderedTypes += $typeOrder | Where-Object { $_ -ne "ClCompile" }

$filtersDoc = New-Object System.Xml.XmlDocument
$xmlDeclaration = $filtersDoc.CreateXmlDeclaration("1.0", "utf-8", $null)
$null = $filtersDoc.AppendChild($xmlDeclaration)

$projectNode = $filtersDoc.CreateElement("Project", $namespaceUri)
$projectNode.SetAttribute("ToolsVersion", "4.0")
$null = $filtersDoc.AppendChild($projectNode)

foreach ($itemType in $orderedTypes) {
    $groupNode = $filtersDoc.CreateElement("ItemGroup", $namespaceUri)
    foreach ($entry in $items | Where-Object { $_.Type -eq $itemType }) {
        $itemNode = $filtersDoc.CreateElement($entry.Type, $namespaceUri)
        $itemNode.SetAttribute("Include", $entry.Include)
        if ($entry.Filter) {
            $filterNode = $filtersDoc.CreateElement("Filter", $namespaceUri)
            $filterNode.InnerText = $entry.Filter
            $null = $itemNode.AppendChild($filterNode)
        }
        $null = $groupNode.AppendChild($itemNode)
    }
    $null = $projectNode.AppendChild($groupNode)

    if ($itemType -eq "ClCompile") {
        $filterGroup = $filtersDoc.CreateElement("ItemGroup", $namespaceUri)
        $sortedFilters = @($filters) | Sort-Object `
            @{ Expression = { ($_ -split '\\').Count } }, `
            @{ Expression = { $_ } }

        foreach ($filter in $sortedFilters) {
            $filterNode = $filtersDoc.CreateElement("Filter", $namespaceUri)
            $filterNode.SetAttribute("Include", $filter)
            $idNode = $filtersDoc.CreateElement("UniqueIdentifier", $namespaceUri)
            
            if ($existingFilterIds.ContainsKey($filter)) {
                $idNode.InnerText = $existingFilterIds[$filter]
            } else {
                $idNode.InnerText = ([guid]::NewGuid().ToString("B"))
            }
            
            $null = $filterNode.AppendChild($idNode)
            $null = $filterGroup.AppendChild($filterNode)
        }
        $null = $projectNode.AppendChild($filterGroup)
    }

}

if (-not $orderedTypes) {
    $filterGroup = $filtersDoc.CreateElement("ItemGroup", $namespaceUri)
    $sortedFilters = @($filters) | Sort-Object `
        @{ Expression = { ($_ -split '\\').Count } }, `
        @{ Expression = { $_ } }

    foreach ($filter in $sortedFilters) {
        $filterNode = $filtersDoc.CreateElement("Filter", $namespaceUri)
        $filterNode.SetAttribute("Include", $filter)
        $idNode = $filtersDoc.CreateElement("UniqueIdentifier", $namespaceUri)
        
        if ($existingFilterIds.ContainsKey($filter)) {
            $idNode.InnerText = $existingFilterIds[$filter]
        } else {
            $idNode.InnerText = ([guid]::NewGuid().ToString("B"))
        }
        
        $null = $filterNode.AppendChild($idNode)
        $null = $filterGroup.AppendChild($filterNode)
    }
    $null = $projectNode.AppendChild($filterGroup)
}


$writerSettings = New-Object System.Xml.XmlWriterSettings
$writerSettings.Indent = $true
$writerSettings.Encoding = New-Object System.Text.UTF8Encoding($false)
$writer = [System.Xml.XmlWriter]::Create($OutputPath, $writerSettings)
$filtersDoc.Save($writer)
$writer.Close()

Write-Host ("Generated filters: {0}" -f $OutputPath)
