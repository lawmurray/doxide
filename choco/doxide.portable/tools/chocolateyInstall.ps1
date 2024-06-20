$ErrorActionPreference = 'Stop'
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  fileType      = 'EXE'
  file64        = "$toolsDir\doxide.exe"
  softwareName  = 'doxide.portable'
  checksum64    = 'ABCDEF0123456789'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
