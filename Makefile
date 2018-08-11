# brownout - simplistic gamma correction
# See LICENSE file for copyright and license details.

include config.mk

SRC = brownout.c is-day.c
OBJ = ${SRC:.c=.o}

all: options brownout is-day

options:
	@echo brownout build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<


config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

${OBJ}: config.h

is-day: is-day.o
	@echo CC -o $@
	@${CC} -o $@ is-day.o ${LDFLAGS}

brownout: brownout.o
	@echo CC -o $@
	@${CC} -o $@ brownout.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f brownout is-day ${OBJ} brownout-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p brownout-${VERSION}
	@cp -R LICENSE Makefile README config.def.h config.mk ${SRC} brownout-${VERSION}
	@tar -cf brownout-${VERSION}.tar brownout-${VERSION}
	@gzip brownout-${VERSION}.tar
	@rm -rf brownout-${VERSION}

install: all
	@echo installing executable files to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f brownout is-day ${DESTDIR}${PREFIX}/bin
	@echo installing manual pages to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < brownout.1 > ${DESTDIR}${MANPREFIX}/man1/brownout.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/brownout.1

uninstall:
	@echo removing executable files from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/brownout
	@rm -f ${DESTDIR}${PREFIX}/bin/is-day
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/brownout.1

.PHONY: all options clean dist install uninstall
