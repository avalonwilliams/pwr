# Copyright (C) 2019 Aidan Williams
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Install variables
PREFIX   ?= /usr/local
BINDIR   ?= bin
MANDIR   ?= man/man1
DOCDIR   ?= share/doc/pwr
BINPERMS ?= 755
DOCPERMS ?= 644

# Build variables
STD      ?= c99
CC       ?= cc

# Allows for dropping in code with minimal modifications to the makefile
SOURCES := $(shell find ./ -name "*.c")


pwr: $(SOURCES)
	$(CC) -std=$(STD) $(CFLAGS) -o pwr $(SOURCES)

debug: $(SOURCES)
	$(CC) -std=$(STD) $(CFLAGS) -g -o pwr $(SOURCES)

clean:
	[ ! -f pwr ] || rm pwr
	[ ! -f doc/pwr.1 ] || rm doc/pwr.1

install: pwr
	install -d $(DESTDIR)$(PREFIX)/$(BINDIR)
	install -m $(BINPERMS) pwr $(DESTDIR)$(PREFIX)/$(BINDIR)
	install -d $(DESTDIR)$(PREFIX)/$(DOCDIR)
	install -m $(DOCPERMS) COPYING $(DESTDIR)$(PREFIX)/$(DOCDIR)
	install -m $(DOCPERMS) README.md $(DESTDIR)$(PREFIX)/$(DOCDIR)
	[ ! -f doc/pwr.1 ] || \
		(install -d $(DESTDIR)$(PREFIX)/$(MANDIR) && \
		install -m $(DOCPERMS) doc/pwr.1 $(DESTDIR)$(PREFIX)/$(MANDIR) && \
		gzip -f $(DESTDIR)$(PREFIX)/$(MANDIR)/pwr.1)

run: pwr
	./pwr

.PHONY: docs
docs: doc/pwr.1.ronn
	ronn -r doc/pwr.1.ronn

