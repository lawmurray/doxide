# Maintainer: Lawrence Murray <lawrence@indii.org>

pkgname=doxide
pkgver=0.0.0
pkgrel=1
pkgdesc="Modern documentation for modern C++"
arch=('x86_64')
url="https://doxide.org"
license=('Apache-2.0')
depends=('libyaml' 'tree-sitter')
makedepends=('cmake')
source=("$pkgname-$pkgver.tar.gz")
md5sums=('SKIP')

build() {
        cd "$srcdir/$pkgname-$pkgver"
        cmake -DCMAKE_BUILD_TYPE=Release . --verbose
        cmake --build . --config Release --verbose
}

check() {
        cd "$srcdir/$pkgname-$pkgver"
        ./doxide help
}

package() {
        cd "$srcdir/$pkgname-$pkgver"
        DESTDIR="$pkgdir" cmake --install .
}
