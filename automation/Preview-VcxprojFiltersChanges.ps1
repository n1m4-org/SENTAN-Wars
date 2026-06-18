Param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$VcxprojPath,
    [string]$OutputPath,
    [switch]$ShowAssignments
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

    $normalized = $normalized -replace '/', '\\'

    foreach ($macro in @('$(ProjectDir)', '$(SolutionDir)')) {
        if ($normalized.StartsWith($macro, [System.StringComparison]::OrdinalIgnoreCase)) {
            $normalized = $normalized.Substring($macro.Length).TrimStart('\\')
            break
        }
    }

    while ($normalized.StartsWith(".\", [System.StringComparison]::OrdinalIgnoreCase)) {
        $normalized = $normalized.Substring(2)
    }

    if ([System.IO.Path]::IsPathRooted($normalized)) {
        $projectPrefix = $ProjectDirectory.TrimEnd('\\')
        if ($normalized.StartsWith($projectPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
            $normalized = $normalized.Substring($projectPrefix.Length).TrimStart('\\')
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
        $path = ($parts[0..$i] -join '\\')
        $null = $FilterSet.Add($path)
    }
}

function Build-ExpectedFiltersDocument {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ResolvedVcxprojPath,
        [Parameter(Mandatory = $true)]
        [string]$ProjectDirectory
    )

    [xml]$projectXml = Get-Content -LiteralPath $ResolvedVcxprojPath -Raw
    $namespaceUri = "http://schemas.microsoft.com/developer/msbuild/2003"
    $namespaceManager = New-Object System.Xml.XmlNamespaceManager($projectXml.NameTable)
    $namespaceManager.AddNamespace("msb", $namespaceUri)

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
        $filterPath = Get-FilterPath -IncludePath $include -ProjectDirectory $ProjectDirectory

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
                $idNode.InnerText = ([guid]::NewGuid().ToString("B"))
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
            $idNode.InnerText = ([guid]::NewGuid().ToString("B"))
            $null = $filterNode.AppendChild($idNode)
            $null = $filterGroup.AppendChild($filterNode)
        }
        $null = $projectNode.AppendChild($filterGroup)
    }

    return [PSCustomObject]@{
        Document = $filtersDoc
        NamespaceUri = $namespaceUri
    }
}

function Get-FiltersDefinitionList {
    param(
        [Parameter(Mandatory = $true)]
        [xml]$FiltersXml,
        [Parameter(Mandatory = $true)]
        [string]$NamespaceUri
    )

    $ns = New-Object System.Xml.XmlNamespaceManager($FiltersXml.NameTable)
    $ns.AddNamespace("msb", $NamespaceUri)

    $nodes = $FiltersXml.SelectNodes("/msb:Project/msb:ItemGroup/msb:Filter[@Include]", $ns)
    $list = @()
    foreach ($n in $nodes) {
        $list += $n.GetAttribute("Include")
    }
    return $list
}

function Get-ItemEntries {
    param(
        [Parameter(Mandatory = $true)]
        [xml]$FiltersXml,
        [Parameter(Mandatory = $true)]
        [string]$NamespaceUri
    )

    $ns = New-Object System.Xml.XmlNamespaceManager($FiltersXml.NameTable)
    $ns.AddNamespace("msb", $NamespaceUri)

    $nodes = $FiltersXml.SelectNodes("/msb:Project/msb:ItemGroup/*[@Include and local-name() != 'Filter']", $ns)
    $items = @()
    foreach ($n in $nodes) {
        $filterNode = $n.SelectSingleNode("msb:Filter", $ns)
        $filterText = $null
        if ($filterNode) {
            $filterText = $filterNode.InnerText
        }

        $items += [PSCustomObject]@{
            Type = $n.LocalName
            Include = $n.GetAttribute("Include")
            Filter = $filterText
        }
    }

    return $items
}

function Show-Assignments {
    param(
        [Parameter(Mandatory = $true)]
        [AllowEmptyCollection()]
        [object[]]$Items
    )

    Write-Host "[登録予定] Filterごとのファイル一覧"
    $groups = $Items |
        Select-Object Type, Include, Filter |
        Group-Object -Property { if ($_.Filter) { $_.Filter } else { "(なし)" } } |
        Sort-Object Name

    foreach ($g in $groups) {
        Write-Host ("== {0} ==" -f $g.Name)
        $g.Group | Sort-Object Type, Include | ForEach-Object {
            Write-Host ("  {0}  {1}" -f $_.Type, $_.Include)
        }
    }
}

$resolvedVcxprojPath = (Resolve-Path -LiteralPath $VcxprojPath).Path
$projectDirectory = Split-Path -Parent $resolvedVcxprojPath
if (-not $OutputPath) {
    $OutputPath = "$resolvedVcxprojPath.filters"
}

$expected = Build-ExpectedFiltersDocument -ResolvedVcxprojPath $resolvedVcxprojPath -ProjectDirectory $projectDirectory
$namespaceUri = $expected.NamespaceUri
$expectedXml = $expected.Document

Write-Host ("対象: {0}" -f $resolvedVcxprojPath)
Write-Host ("比較先(.filters): {0}" -f $OutputPath)

if (-not (Test-Path -LiteralPath $OutputPath)) {
    Write-Host "結果: .filters が存在しません。Generate-VcxprojFilters.ps1 は新規作成します。"
    $expectedFilters = @(Get-FiltersDefinitionList -FiltersXml $expectedXml -NamespaceUri $namespaceUri)
    $expectedItems = @(Get-ItemEntries -FiltersXml $expectedXml -NamespaceUri $namespaceUri)
    Write-Host ("生成予定: フィルタ定義 {0} 件 / アイテム {1} 件" -f $expectedFilters.Count, $expectedItems.Count)

    if ($ShowAssignments) {
        Show-Assignments -Items $expectedItems
    }

    exit 1
}

[xml]$actualXml = Get-Content -LiteralPath $OutputPath -Raw

$expectedFilters = @(Get-FiltersDefinitionList -FiltersXml $expectedXml -NamespaceUri $namespaceUri)
$actualFilters = @(Get-FiltersDefinitionList -FiltersXml $actualXml -NamespaceUri $namespaceUri)

$expectedItems = @(Get-ItemEntries -FiltersXml $expectedXml -NamespaceUri $namespaceUri)
$actualItems = @(Get-ItemEntries -FiltersXml $actualXml -NamespaceUri $namespaceUri)

if ($ShowAssignments) {
    Show-Assignments -Items $expectedItems
}

$filterDiff = Compare-Object -ReferenceObject ($actualFilters | Sort-Object -Unique) -DifferenceObject ($expectedFilters | Sort-Object -Unique)

function Key-Item($it) {
    return "{0}|{1}" -f $it.Type, $it.Include
}

$actualByKey = @{}
foreach ($it in $actualItems) {
    $k = Key-Item $it
    if (-not $actualByKey.ContainsKey($k)) { $actualByKey[$k] = @() }
    $actualByKey[$k] += $it
}

$expectedByKey = @{}
foreach ($it in $expectedItems) {
    $k = Key-Item $it
    if (-not $expectedByKey.ContainsKey($k)) { $expectedByKey[$k] = @() }
    $expectedByKey[$k] += $it
}

$allKeys = @($actualByKey.Keys + $expectedByKey.Keys) | Sort-Object -Unique

$itemsAdded = @()
$itemsRemoved = @()
$itemsMoved = @()

foreach ($k in $allKeys) {
    $a = @()
    if ($actualByKey.ContainsKey($k)) { $a = $actualByKey[$k] }
    $e = @()
    if ($expectedByKey.ContainsKey($k)) { $e = $expectedByKey[$k] }

    if ($a.Count -eq 0 -and $e.Count -gt 0) {
        foreach ($x in $e) { $itemsAdded += $x }
        continue
    }
    if ($a.Count -gt 0 -and $e.Count -eq 0) {
        foreach ($x in $a) { $itemsRemoved += $x }
        continue
    }

    foreach ($ai in $a) {
        $match = $e | Where-Object { $_.Filter -eq $ai.Filter } | Select-Object -First 1
        if (-not $match) {
            $new = $e | Select-Object -First 1
            $itemsMoved += [PSCustomObject]@{
                Type = $ai.Type
                Include = $ai.Include
                From = $ai.Filter
                To = $new.Filter
            }
        }
    }
}

$hasChanges = $false

if ($filterDiff.Count -gt 0) {
    $hasChanges = $true
    $adds = @($filterDiff | Where-Object SideIndicator -eq '=>' | Select-Object -ExpandProperty InputObject)
    $removes = @($filterDiff | Where-Object SideIndicator -eq '<=' | Select-Object -ExpandProperty InputObject)

    if ($adds.Count -gt 0) {
        Write-Host "[フィルタ定義] 追加予定:"
        $adds | Sort-Object | ForEach-Object { Write-Host ("  + {0}" -f $_) }
    }
    if ($removes.Count -gt 0) {
        Write-Host "[フィルタ定義] 削除予定:"
        $removes | Sort-Object | ForEach-Object { Write-Host ("  - {0}" -f $_) }
    }
}

if ($itemsAdded.Count -gt 0) {
    $hasChanges = $true
    Write-Host "[アイテム] 追加予定:"
    $itemsAdded | Sort-Object Type, Include | ForEach-Object {
        $f = if ($_.Filter) { $_.Filter } else { "(なし)" }
        Write-Host ("  + {0}  Include={1}  Filter={2}" -f $_.Type, $_.Include, $f)
    }
}

if ($itemsRemoved.Count -gt 0) {
    $hasChanges = $true
    Write-Host "[アイテム] 削除予定:"
    $itemsRemoved | Sort-Object Type, Include | ForEach-Object {
        $f = if ($_.Filter) { $_.Filter } else { "(なし)" }
        Write-Host ("  - {0}  Include={1}  Filter={2}" -f $_.Type, $_.Include, $f)
    }
}

if ($itemsMoved.Count -gt 0) {
    $hasChanges = $true
    Write-Host "[アイテム] Filter変更(移動)予定:"
    $itemsMoved | Sort-Object Type, Include | ForEach-Object {
        $from = if ($_.From) { $_.From } else { "(なし)" }
        $to = if ($_.To) { $_.To } else { "(なし)" }
        Write-Host ("  * {0}  Include={1}  {2} -> {3}" -f $_.Type, $_.Include, $from, $to)
    }
}

if (-not $hasChanges) {
    Write-Host "結果: 変更はありません (Generate-VcxprojFilters.ps1 を実行しても内容は同一になります。)"
    exit 0
}

Write-Host "結果: 変更があります (Generate-VcxprojFilters.ps1 を実行すると .filters が更新されます。)"


exit 1
