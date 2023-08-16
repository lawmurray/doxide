# Download

Follow the instructions below for your operating system.

## :simple-ubuntu: Ubuntu

??? info "Ubuntu 23.04 Lunar Lobster (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb lunar main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

??? info "Ubuntu 22.10 Kinetic Kudu (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb kinetic main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

??? info "Ubuntu 22.04 Jammy Jellyfish (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb jammy main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

??? info "Ubuntu 20.04 Focal Fossa (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb focal main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

## :simple-debian: Debian

??? info "Debian 12 Bookworm (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb bookworm main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

??? info "Debian 11 Bullseye (amd64)"
    Install Doxide:
    ```
    echo 'deb http://download.indii.org/deb bullseye main' | sudo tee /etc/apt/sources.list.d/indii.org.list
    curl -fsSL https://download.indii.org/deb/Release.key | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/indii.org.gpg > /dev/null
    sudo apt update
    sudo apt install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

## :simple-fedora: Fedora

??? info "Fedora 38 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/38/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

??? info "Fedora 37 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/37/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

??? info "Fedora 36 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/36/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

??? info "Fedora 35 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/fedora/35/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

## :simple-opensuse: openSUSE

??? info "openSUSE Tumbleweed (x86_64)"
    Install Doxide:
    ```
    sudo zypper addrepo https://download.indii.org/rpm/opensuse/tumbleweed/indii.org.repo
    sudo zypper refresh
    sudo zypper install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

## :simple-linux: Mageia

??? info "Mageia 8 (x86_64)"
    Install Doxide:
    ```
    sudo dnf config-manager --add-repo https://download.indii.org/rpm/mageia/8/indii.org.repo
    sudo dnf update
    sudo dnf install doxide
    ```
    Install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

## :simple-archlinux: Arch

Available from the [Arch User Repository (AUR)](https://aur.archlinux.org/packages/doxide).

## :simple-apple: Mac

??? info "Homebrew"
    Install [Homebrew](https://brew.sh) if not already, then:
    ```sh
    brew tap lawmurray/all
    brew install doxide
    ```
    Also install [MkDocs][mkdocs] and [Material for MkDocs][material]:
    ```
    pip install mkdocs mkdocs-material
    ```

## :simple-windows: Windows

??? info "Chocolatey (recommended)"
    Install [Chocolatey](https://chocolatey.org) if not already, then:
    ```sh
    choco install doxide mkdocs mkdocs-material
    ```
    
??? info "Installer"
    [:material-download: Download Doxide installer](https://download.indii.org/win/doxide-installer.exe){ .md-button }
    
    If you have a working [Python](https://python.org) environment, install [MkDocs][mkdocs] and [Material for MkDocs][material] with:
    ```
    pip install mkdocs mkdocs-material
    ```
    [Checksum](https://download.indii.org/win/doxide-installer.exe.sha256) for the download above.

??? info "Standalone program"
    Download the program and put it somewhere on your `PATH` environment variable, so that you can access it from a terminal.

    [:material-download: Download Doxide standalone program](https://download.indii.org/win/doxide.exe){ .md-button }

    If you have a working [Python](https://python.org) environment, install [MkDocs][mkdocs] and [Material for MkDocs][material] with:
    ```
    pip install mkdocs mkdocs-material
    ```
    [Checksum](https://download.indii.org/win/doxide.exe.sha256) for the download above.

??? info "Windows Subsystem for Linux"
    You can use [Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install) with one of the Linux distributions above and follow the instructions for it.

## :fontawesome-solid-file-zipper: Others: Install from source

If a package is not available for your operating system or you have special requirements, you can install from source. See the [README.md](https://github.com/lawmurray/doxide) file for instructions.

[mkdocs]: https://www.mkdocs.org
[material]: https://squidfunk.github.io/mkdocs-material/
