Name: doxide
Version: 0.0.0
Release: 1
Summary: Modern documentation for modern C++
Vendor: Lawrence Murray <lawrence@indii.org>
License: Apache-2.0
Group: Development/Languages/C and C++
URL: https://doxide.org
Source0: %{name}-%{version}.tar.gz
BuildRequires: gcc-c++ cmake libyaml-devel libicu-devel

%description

Doxide generates documentation for C++ source code. It is configured with
YAML, generates Markdown, and publishes HTML. Entities in the source code are
documented with special comments containing commands, as with the classic tool
Doxygen. The source code is parsed and documentation processed into Markdown
then HTML. Doxide aims at online documentation with a modern look and
responsive design for desktop and mobile devices.

%prep
%setup -q -n %{name}-%{version}

%build
%cmake
%cmake_build

%install
%cmake_install

%files
%license LICENSE
%{_bindir}/%{name}

%changelog
%autochangelog
