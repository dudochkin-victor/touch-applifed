# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.20
# 
# >> macros
# << macros

Name:       meegotouch-applifed
Summary:    Application life cycle daemon
Version:    0.20.0
Release:    1
Group:      System/Daemons
License:    LGPLv2+
URL:        http://meego.gitorious.com/meegotouch/meegotouch-applifed
Source0:    %{name}-%{version}.tar.bz2
Source100:  meegotouch-applifed.yaml
Patch0:     0001-Use-usr-bin-ruby-instead-of-usr-bin-ruby1.8.patch
Patch1:     0002-Ship-empty-conf-file-in-MeeGo.patch
Patch2:     0003-Install-.desktop-file-on-MeeGo.patch
BuildRequires:  pkgconfig(QtGui)
BuildRequires:  pkgconfig(contextprovider-1.0)
BuildRequires:  pkgconfig(meegotouch)
BuildRequires:  pkgconfig(x11)
BuildRequires:  cmake


%description
applifed is an application life cycle daemon. It manages prestarted applications.


%package tests
Summary:    Tests for meegotouch-applifed
Group:      Development/Tools
Requires:   %{name} = %{version}-%{release}
BuildRequires:   desktop-file-utils

%description tests
Testing related files for meegotouch-applifed.



%prep
%setup -q -n %{name}-%{version}

# 0001-Use-usr-bin-ruby-instead-of-usr-bin-ruby1.8.patch
%patch0 -p1
# 0002-Ship-empty-conf-file-in-MeeGo.patch
%patch1 -p1
# 0003-Install-.desktop-file-on-MeeGo.patch
%patch2 -p1
# >> setup
# << setup

%build
# >> build pre
export BUILD_TESTS=1
export MEEGO=1
# << build pre

%configure --disable-static
make %{?jobs:-j%jobs}

# >> build post
# << build post
%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%make_install

# >> install post
# rpmlint complains about installing binaries in /usr/share, so
# move them elsewhere and leave a symlink in place.
mkdir -p %{buildroot}/usr/lib
mv %{buildroot}/usr/share/applifed-tests %{buildroot}/usr/lib
(cd %{buildroot}/usr/share; ln -s ../lib/applifed-tests)
# << install post









%files
%defattr(-,root,root,-)
%{_bindir}/applifed
%config %{_sysconfdir}/prestart/nokia.conf
%config %{_sysconfdir}/xdg/autostart/applifed.desktop
# >> files
# << files


%files tests
%defattr(-,root,root,-)
%{_bindir}/applifed
%{_bindir}/close-event
%{_bindir}/fali_hello
%{_bindir}/fali_multiapp
%{_bindir}/fali_mw1
%{_bindir}/fali_mw2
%{_bindir}/fali_mw3
%{_bindir}/fali_perf
%{_bindir}/fali_toc
%{_bindir}/memoryhog
%{_datadir}/applications/fali_hello.desktop
%{_datadir}/applications/fali_toc.desktop
%{_datadir}/applifed-art-tests/tests.xml
%{_datadir}/applifed-bug-tests/tests.xml
%{_datadir}/applifed-functional-tests/tests.xml
%{_datadir}/applifed-performance-tests/tests.xml
%{_datadir}/applifed-tests
%{_libdir}/applifed-tests/tests.xml
%{_libdir}/applifed-tests/ut_configdata
%{_libdir}/applifed-tests/ut_cpuload
%{_libdir}/applifed-tests/ut_prestartapp
%{_libdir}/applifed-tests/ut_prestarter
%{_datadir}/applifed-testscripts/cp-service.rb
%{_datadir}/applifed-testscripts/cpu_load/cpu_load.conf
%{_datadir}/applifed-testscripts/cpu_load/cpu_load_high.drive
%{_datadir}/applifed-testscripts/cpu_load/cpu_load_low.drive
%{_datadir}/applifed-testscripts/cpu_load/tc_cpu_load.rb
%{_datadir}/applifed-testscripts/fautils.rb
%{_datadir}/applifed-testscripts/functest_applifed.conf
%{_datadir}/applifed-testscripts/functional.drive
%{_datadir}/applifed-testscripts/periodic_re_prestart/periodic_re_prestart.conf
%{_datadir}/applifed-testscripts/periodic_re_prestart/periodic_re_prestart.drive
%{_datadir}/applifed-testscripts/periodic_re_prestart/ts_periodic_re_prestart.rb
%{_datadir}/applifed-testscripts/prevent.conf
%{_datadir}/applifed-testscripts/prevent.drive
%{_datadir}/applifed-testscripts/sbservice/com.nokia.fali_hello.service.sb
%{_datadir}/applifed-testscripts/sbservice/com.nokia.fali_toc.service.sb
%{_datadir}/applifed-testscripts/tc_applifed_one_instance.rb
%{_datadir}/applifed-testscripts/tc_daemonize.rb
%{_datadir}/applifed-testscripts/tc_prestart_preventing.rb
%{_datadir}/applifed-testscripts/test-perf-prestart.py
%{_datadir}/applifed-testscripts/test-perf-prestart.pyc
%{_datadir}/applifed-testscripts/test-perf-prestart.pyo
%{_datadir}/applifed-testscripts/ts_mwtests.rb
%{_datadir}/applifed-testscripts/ts_prestart.rb
%{_datadir}/dbus-1/services/com.nokia.fali_hello.service
%{_datadir}/dbus-1/services/com.nokia.fali_mw1.service
%{_datadir}/dbus-1/services/com.nokia.fali_mw2.service
%{_datadir}/dbus-1/services/com.nokia.fali_mw3.service
%{_datadir}/dbus-1/services/com.nokia.fali_perf.service
%{_datadir}/dbus-1/services/com.nokia.fali_toc.service
# >> files tests
# << files tests

