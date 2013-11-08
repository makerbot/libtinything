Name:		libtinything
Version:	$libtinything
Release:	1%{?dist}
Summary:	MakerBot tinything implementation lib

License:	Proprietary
URL:		http://www.makerbot.com/makerware
Source:	        %{name}-%{version}.tar.gz

BuildRequires:	gcc, jsoncpp-devel >= 0.6.0
Requires:	jsoncpp >= 0.6.0


%description
Library encapsulating the .makerbot format

%prep
%setup -q -n %{name}


%build
scons --no-devel-libs --install-prefix=%{buildroot}/%{_prefix} 


%install
scons --no-devel-libs --install-prefix=%{buildroot}/%{_prefix} install

if [ ! -d %{buildroot}/%{_libdir} ]; then
    mv %{buildroot}/%{_prefix}/lib %{buildroot}/%{_libdir}
fi

%files
%{_libdir}/*


%package devel
Summary:	Developent files for MakerBot tinything library
Requires:	libtinything = $libtinything

%description devel
Library encapsulating the .makerbot format

%files devel
%{_includedir}/*


%changelog

