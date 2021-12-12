ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

all: install

install:
ifneq ($(shell id -u), 0)
	@echo "You must be root to perform this action."
else
	mkdir -p ./build
	mkdir -p /data/yah
	cd ${ROOT_DIR}/build && cmake ${ROOT_DIR} && make && cd -
	cp ${ROOT_DIR}/build/yah /usr/bin/yah
	cp ${ROOT_DIR}/build/btl /usr/bin/btl

	systemctl daemon-reload
	systemctl stop yah || true
	systemctl disable yah || true
	systemctl stop etl || true
	systemctl disable etl || true

	cp ${ROOT_DIR}/systemd/yah.service /usr/lib/systemd/system/yah.service
	cp ${ROOT_DIR}/systemd/btl.service /usr/lib/systemd/system/btl.service
	cp ${ROOT_DIR}/yah.conf /etc/yah.conf

	systemctl enable yah
	systemctl enable btl

	@echo
	@echo "Install done. Binary file added to /usr/bin/yah."
	@echo "Please run \"systemctl start yah\" to start the service."
	@echo "Try \"yah\" to check the installation"
endif

uninstall:
ifneq ($(shell id -u), 0)
	@echo "You must be root to perform this action."
else
	systemctl daemon-reload
	systemctl stop yah || true
	systemctl disable yah || true
	rm /usr/lib/systemd/system/yah.service || true
	rm /usr/bin/yah || true
	rm /etc/yah.conf || true
	systemctl daemon-reload

	@echo
	@echo "Uninstall done."
endif
