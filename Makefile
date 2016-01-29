# brownout - simplistic gamma correction
# See LICENSE file for copyright and license details.

include config.mk

SRC = brownout.c
OBJ = ${SRC:.c=.o}

all: options brownout

options:
	@echo brownout build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

brownout: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f brownout ${OBJ} brownout-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p brownout-${VERSION}
	@cp -R LICENSE Makefile README config.def.h config.mk ${SRC} brownout-${VERSION}
	@tar -cf brownout-${VERSION}.tar brownout-${VERSION}
	@gzip brownout-${VERSION}.tar
	@rm -rf brownout-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f brownout ${DESTDIR}${PREFIX}/bin

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/brownout

.PHONY: all options clean dist install uninstall
