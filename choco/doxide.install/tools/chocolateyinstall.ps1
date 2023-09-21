$ErrorActionPreference = 'Stop'
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  fileType      = 'EXE'
  file64        = "$toolsDir\doxide-installer.exe"
  softwareName  = 'doxide.install'
  checksum64    = 'ABCDEF0123456789'
  checksumType64= 'sha256'
  silentArgs   = '/S /DADD_TO_PATH_ALL_USERS=1'
}

Install-ChocolateyInstallPackage @packageArgs
