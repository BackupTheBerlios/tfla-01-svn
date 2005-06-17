Summary: 	The Fabulous Logic Analyzer
Name: 		tfla-01
Version: 	0.1.2
Release: 	0
Packager:       Bernhard Walle <bernhard.walle@gmx.de>
License:	GPL
URL: 		http://tfla-01.berlios.de
Source0: 	%{name}-%{version}.tar.gz
Group: 		Applications/Engineering
BuildRoot: 	%{_tmppath}/%{name}-root
Requires:	qt3 >= 3.3.0 
Requires:	libieee1284 
BuildRequires:	qt3-devel >= 3.3.0
BuildRequires: 	libieee1284-devel

%description
Simple logic analyzer for the PC's parallel port. The schematic which
needs to be attached on the parallel port is included in this installation
package.

Author
------
  Bernhard Walle <bernhard.walle@gmx.de>

%prep
%setup -q

%build
qmake PREFIX=$RPM_BUILD_ROOT/%{_prefix} tfla-01.pro debug=1
lrelease tfla-01.pro
make

%install
rm -rf $RPM_BUILD_ROOT
# running again because the binary file must exist to be recognized
qmake PREFIX=$RPM_BUILD_ROOT/%{_prefix} tfla-01.pro 
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc %{_datadir}/doc/packages/tfla-01
%{_bindir}/*
%{_datadir}/tfla-01
%{_datadir}/pixmaps/*.png
