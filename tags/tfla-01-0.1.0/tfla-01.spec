Summary: 	The Fabulous Logic Analyzer
Name: 		tfla-01
Version: 	0.1.0
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
qmake tfla-01.pro debug=1
lrelease tfla-01.pro
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_bindir} 
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/tfla-01
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/pixmaps
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/tfla-01/translations
install -m 0755 tfla-01 $RPM_BUILD_ROOT/%{_bindir}
install -m 0644 images/tfla-01_*.png $RPM_BUILD_ROOT/%{_datadir}/pixmaps/
install -m 0644 COPYING $RPM_BUILD_ROOT/%{_datadir}/tfla-01/
install -m 0644 ts/*.qm $RPM_BUILD_ROOT/%{_datadir}/tfla-01/translations
#ln -s %{_docdir}/qpamat $RPM_BUILD_ROOT/%{_datadir}/qpamat/doc

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README COPYING doc/schematic/*.pdf
%{_bindir}/*
%{_datadir}/tfla-01
%{_datadir}/pixmaps/*.png
