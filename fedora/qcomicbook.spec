Summary: A viewer for comic book archives (rar, cbr, cbz, zip, ace, cba, tar.gz, tar.bz2)
Name: qcomicbook
Version: 0.1.8
Release: 1.FC3
License: GPL
Group: Amusements/Graphics
URL: http://linux.bydg.org/~yogin
Source0: qcomicbook-%{version}.tar.gz
Source1: qcomicbook.desktop
Source2: qcomicbook.png

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildArch: i386

%description
QComicBook is a viewer for comic book archives containing jpeg/png images, which aims at
convenience and simplicity. Features include: automatic decompression, full-screen mode,
two pages mode, japanese mode, thumbnails view, page scaling, page preloading and caching,
mouse or keyboard navigation etc.

%prep
%setup

%build
./configure --prefix=/usr --mandir=/usr/share/man
make

%install
rm -Rf %{buildroot}
make install DESTDIR=%{buildroot}
mkdir -p %{buildroot}%{_docdir}/qcomicbook-%{version}
cp AUTHORS README ChangeLog COPYING THANKS TODO %{buildroot}%{_docdir}/qcomicbook-%{version}/
mkdir -p %{buildroot}%{_datadir}/{applications,pixmaps}
cp -a %{SOURCE1} %{buildroot}%{_datadir}/applications
cp -a %{SOURCE2} %{buildroot}%{_datadir}/pixmaps

%clean
rm -Rf %{buildroot}

%files
%defattr(-,root,root,-)
/usr/bin/qcomicbook
%{_docdir}/qcomicbook-%{version}/*
%{_datadir}/applications/*
%{_datadir}/qcomicbook/*
%{_datadir}/pixmaps/*
%{_datadir}/man/man1/*
