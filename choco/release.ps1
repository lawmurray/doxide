function Find-Replace {
    param($Path, $Find, $Replace)
    $contents = Get-Content -Path $Path
    $contents = $contents -replace $Find, $Replace
    $contents | Set-Content -Path $Path
}

function Update-Package {
    param($Package, $Download)
    if ($Download -ne $null) {
        Invoke-WebRequest -Uri "https://download.indii.org/win/$Download" -OutFile "$Package\tools\$Download"
        Invoke-WebRequest -Uri "https://download.indii.org/win/$Download.sha256" -OutFile "$Package\tools\$Download.sha256"
        Find-Replace -Path "$Package\tools\chocolateyinstall.ps1" ABCDEF0123456789 $(Get-Content -Path "$Package\tools\$Download.sha256")
    }
    Find-Replace -Path "$Package\$Package.nuspec" 0.0.0 $version
}

function Build-Package {
    param($Package, $Version)
    Set-Location -Path $Package
    choco pack --version $Version
    Set-Location -Path ..
}

function Test-Package {
    param($Package, $Version)
    Set-Location -Path $Package
    choco install $Package --version $Version --source .
    choco uninstall $Package
    Set-Location -Path ..
}

function Release-Package {
    param($Package, $Version)
    Set-Location -Path $Package
    choco push $Package.$Version.nupkg --source "https://push.chocolatey.org/"
    Set-Location -Path ..
}

$version=$(git describe --tags --abbrev=0).Substring(1)
$choco_version=$(choco search --limit-output --exact doxide).Substring(7)

if ($version -ne $choco_version) {
    Update-Package -Package doxide.portable -Download doxide.exe
    Update-Package -Package doxide.install -Download doxide-installer.exe
    Update-Package -Package doxide

    Build-Package -Package doxide.portable -Version $version
    Build-Package -Package doxide.install -Version $version
    Build-Package -Package doxide -Version $version

    Test-Package -Package doxide.portable -Version $version
    Test-Package -Package doxide.install -Version $version
    #Test-Package -Package doxide -Version $version

    Release-Package -Package doxide.portable -Version $version
    Release-Package -Package doxide.install -Version $version
    Release-Package -Package doxide -Version $version
}
