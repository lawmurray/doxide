Name: birch
Version: 0.0.0
Release: 1
Summary: A universal probabilistic programming language
Vendor: Lawrence Murray <lawrence@indii.org>
License: Apache-2.0
Group: Development/Languages/C and C++
URL: https://birch.sh
Source0: %{name}-%{version}.tar.gz

%if 0%{?suse_version} || 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version}
BuildRequires: flex bison gcc-c++ autoconf automake libtool libyaml-devel jemalloc-devel
%endif
%if 0%{?mageia}
BuildRequires: flex bison gcc-c++ autoconf automake libtool yaml-devel jemalloc-devel
%endif
Recommends: gcc-c++ make autoconf automake libtool binutils elfutils numbirch-devel == %{version} membirch-devel == %{version} birch-standard-devel == %{version}

%description
Birch is a programming language for expressing probabilistic models and
performing Bayesian inference. It is used by statisticians, data scientists,
and machine learning engineers. Its features include automatic
differentiation, automatic marginalization, and automatic conditioning. These
compose into advanced Monte Carlo inference algorithms. The Birch language
transpiles to C++, with multithreading CPU support, GPU support, and fast
copy-on-write memory management.

%prep
%setup -q -n %{name}-%{version}

%build
%configure --disable-assert
%make_build
strip --strip-unneeded %{name}

%install
%make_install

%files
%license LICENSE
%{_bindir}/%{name}
%{_datadir}/%{name}

# address rpmlint error
%attr(755,-,-) %{_datadir}/%{name}/bootstrap

%changelog
* Fri Dec 2 2022 Lawrence Murray <lawrence@indii.org> - 1:0.0.0-1
Initial setup.
