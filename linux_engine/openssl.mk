include ../.config

ifdef TOOLCHAIN_ABI_N32
   SSL_TARGET=linux-octeon32
endif
ifdef TOOLCHAIN_ABI_64
   SSL_TARGET=linux-octeon64
endif

PKG:=openssl
OPENSSL_VERSION?=1.0.1g
DIR:=${PKG}-${OPENSSL_VERSION}

.PHONY: all
all: build install

.PHONY: build
build: ${DIR} ${DIR}/Makefile
	${MAKE} -C ${DIR}

${DIR}/Makefile:
	cd ${DIR} && ./Configure shared --openssldir=/usr ${SSL_TARGET}

.PHONY: install
install: ${DIR}
	${MAKE} -C ${DIR} INSTALL_PREFIX=${ROOT} install_sw

${DIR}:
	tar -zxf ${STORAGE}/${PKG}-${OPENSSL_VERSION}.tar.gz
	cd ${DIR} && patch -p0 < ${STORAGE}/openssl.patch
	cd ${DIR} && rm -f ./Makefile
