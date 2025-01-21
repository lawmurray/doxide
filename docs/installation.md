# Installing Doxide

Follow the instructions below for your operating system.

## :simple-linux: Linux

Install from the [software repository](https://download.indii.org), following the instructions provided there.

Alternatively, for Arch Linux and derivatives (e.g. EndeavourOS, Garuda, Manjaro), install Doxide from the [Arch User Repository (AUR)](https://aur.archlinux.org/packages/doxide).


Also install [MkDocs][mkdocs] and [Material for MkDocs][material]:
```
pip install mkdocs mkdocs-material
```
Some Linux distributions include Material for MkDocs as a package too. It is typically an older version, but may be sufficient for you.

[See below](#using-a-python-virtual-environment) for running MkDocs in a Python virtual environment instead.

## :simple-apple: Mac

Install [Homebrew](https://brew.sh) if not already, then:
```sh
brew tap lawmurray/all
brew install doxide
```
Also install [MkDocs][mkdocs] and [Material for MkDocs][material]:
```
pip install mkdocs mkdocs-material
```
[See below](#using-a-python-virtual-environment) for running MkDocs in a Python virtual environment instead.

## :fontawesome-brands-windows: Windows

First install Doxide via your preferred means:

??? info "Chocolatey"
    Install [Chocolatey](https://chocolatey.org) if not already, then:
    ```sh
    choco install doxide
    ```
    
??? info "Installer"
    [:material-download: Download Doxide installer](https://download.indii.org/win/doxide-installer.exe){ .md-button }

    [Checksum](https://download.indii.org/win/doxide-installer.exe.sha256) for the download above.

??? info "Standalone program"
    Download the program and put it somewhere on your `PATH` environment variable, so that you can access it from a terminal.

    [:material-download: Download Doxide standalone program](https://download.indii.org/win/doxide.exe){ .md-button }

    [Checksum](https://download.indii.org/win/doxide.exe.sha256) for the download above.

??? info "Windows Subsystem for Linux"
    You can use [Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install) and follow the instructions for Linux above.

Then install [MkDocs][mkdocs] and [Material for MkDocs][material]. For this you will need a working [Python](https://python.org) installation; if you are using [Chocolatey](https://chocolatey.org), a simple way is `choco install python`. Then install:
```
pip install mkdocs mkdocs-material
```
[See below](#using-a-python-virtual-environment) for running MkDocs in a Python virtual environment instead.

## :fontawesome-solid-file-zipper: Install from source

If a Doxide package is not available for your operating system or you have special requirements, you can install from source. See the [README.md](https://github.com/lawmurray/doxide) file for instructions.

[mkdocs]: https://www.mkdocs.org
[material]: https://squidfunk.github.io/mkdocs-material/

## :simple-python: Using a Python virtual environment

To manage MkDocs and other Python dependencies per-project rather than system wide, you may like to use a Python virtual environment. Create it in your project directory:
```
python3 -m venv venv
```
Then enter it:
```
source venv/bin/activate
```
You can then run  `pip install mkdocs mkdocs-material` to install MkDocs (and other dependencies) into this virtual environment, and run `mkdocs` from within that virtual environment to publish your Doxide documentation. When finished, exit the environment with:
```
deactivate
```
