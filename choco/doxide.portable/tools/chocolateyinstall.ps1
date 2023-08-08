$ErrorActionPreference = 'Stop'
$toolsDir   = "$(Split-Path -parent $MyInvocation.MyCommand.Definition)"

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  unzipLocation = $toolsDir
  fileType      = 'EXE'
  file64        = "$toolsDir\doxide.exe"
  softwareName  = 'doxide.portable'
  checksum64    = '56C3E89F0ACF66CD4641D2A25AFF45952C561112B0AE1D2F7C5126F50B4DB879'
  checksumType64= 'sha256'
}

Install-ChocolateyZipPackage @packageArgs
