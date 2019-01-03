# vim: set noexpandtab tabstop=4 shiftwidth=4:

# Copyright (C) 2019 Aidan Williams
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

pwr: pwr.c pwr.h
	gcc $(DEBUG) -o pwr pwr.c pwr.h

clean:
	rm pwr

install: pwr
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 pwr $(DESTDIR)$(PREFIX)/bin

run: pwr
	./pwr
