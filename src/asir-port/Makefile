# $OpenXM$

all:
install: all
	@echo "Installing scripts for portaging asir by downloading."
	install -m 755 asir-port.sh ../../bin

install-for-knoppix:
	rm ../../bin/asir
	ln -s /home/knoppix/.asir-tmp/asir ../../bin/asir
