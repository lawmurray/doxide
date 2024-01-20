# Installing Doxide

Follow the instructions below for your operating system.

## :simple-linux: Linux

Install Doxide from your package manager by adding appropriate repository for your Linux distribution below. If your distribution is not listed, you can instead [install from source](#install-from-source).

??? info ":simple-ubuntu: Ubuntu 23.10 Mantic Minotaur (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb mantic main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```

??? info ":simple-ubuntu: Ubuntu 23.04 Lunar Lobster (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb lunar main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```

??? info ":simple-ubuntu: Ubuntu 22.04 Jammy Jellyfish (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb jammy main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```

??? info ":simple-ubuntu: Ubuntu 20.04 Focal Fossa (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb focal main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```

??? info ":simple-debian: Debian 12 Bookworm (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb bookworm main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```

??? info ":simple-debian: Debian 11 Bullseye (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb bullseye main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```

??? info ":simple-fedora: Fedora 39 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/39/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```

??? info ":simple-fedora: Fedora 38 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/38/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```

??? info ":simple-fedora: Fedora 37 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/37/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```

??? info ":simple-fedora: Fedora 36 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/36/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```

??? info ":simple-fedora: Fedora 35 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/35/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```

??? info ":simple-opensuse: openSUSE Tumbleweed (x86_64)"
    Install Doxide:
    ```
    sudo zypper addrepo https://download.indii.org/rpm/opensuse/tumbleweed/indii.org.repo
    sudo zypper refresh
    sudo zypper install doxide
    ```

??? info ":simple-archlinux: Arch"
    Available from the [Arch User Repository (AUR)](https://aur.archlinux.org/packages/doxide).

Then install [MkDocs][mkdocs] and [Material for MkDocs][material]:
```
pip install mkdocs mkdocs-material
```
[See below](#using-a-python-virtual-environment) for running MkDocs in a Python virtual environment instead.

!!! tip
    Material for MkDocs is included as a package in some Linux distributions, but typically an older version, and mileage may vary when using it with Doxide. As Material for MkDocs is updated frequently, it is recommended to install it with `pip` instead, to get the latest version.

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

## :simple-windows: Windows

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
    You can use [Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install) and follow the instructions for your chosen Linux distribution above.

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
You can then run  `pip install mkdocs mkdocs-material` to install MkDocs (or other dependencies) into this virtual environment, and run `mkdocs` from within that virtual environment to publish your Doxide documentation.
