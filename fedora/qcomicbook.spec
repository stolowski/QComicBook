Summary: A viewer for comic book archives (rar, cbr, cbz, zip, ace, cba, tar.gz, tar.bz2)
Name: qcomicbook
Version: 0.3.0
Release: 1
License: GPL
Group: Amusements/Graphics
URL: http://linux.bydg.org/~yogin
Packager: Pawel Stolowski <pawel.stolowski@wp.pl>
Vendor:	Pawel Stolowski <pawel.stolowski@wp.pl>
Source0: http://linux.bydg.org/~yogin/%{name}-%{version}.tar.gz
Source1: %{name}.desktop
Source2: %{name}.png

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildArch: i386
BuildRequires: qt-devel >= 3.3.1
BuildRequires: imlib2-devel >= 1.2.0

Prefix: %{_prefix}
Requires: qt >= 3.3.1

%description
QComicBook is a viewer for comic book archives (rar, cbr, zip, cbz, ace, cba,
tar.gz, cbg, tar.bz2, cbb) containing jpeg/png images, which aims at convenience
and simplicity. Features include: automatic decompression, full-screen mode, two
pages mode, japanese mode, thumbnails view, page scaling and rotating, page
preloading and caching, mouse or keyboard navigation, bookmarks etc.

QComicBook requires zip/unzip, rar/unrar, tar with gzip+bzip2 support and unace 
to handle archives.

%prep
%setup -q

%build
%{configure}
%{__make}

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf "$RPM_BUILD_ROOT"
%{__make} install \
	DESTDIR="$RPM_BUILD_ROOT"

mkdir -p "$RPM_BUILD_ROOT"%{_datadir}/{applications,pixmaps}
cp -a %{SOURCE1} "$RPM_BUILD_ROOT"%{_datadir}/applications
cp -a %{SOURCE2} "$RPM_BUILD_ROOT"%{_datadir}/pixmaps

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf "$RPM_BUILD_ROOT"

%files
%defattr(-,root,root,-)
%doc AUTHORS README ChangeLog COPYING THANKS TODO
%docdir %{_datadir}/%{name}/help
%{_bindir}/%{name}
%{_datadir}/%{name}
%{_mandir}/man1/*
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}*

