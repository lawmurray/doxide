$ErrorActionPreference = 'Stop'

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  softwareName  = 'doxide.install'
  fileType      = 'EXE'
  silentArgs   = '/S'
  file          = 'C:\Program Files\Doxide\Uninstall.exe'
}

Uninstall-ChocolateyPackage @packageArgs
