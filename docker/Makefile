PWD = $(shell pwd)
HACKOORT_TAG = $(shell ./pi.sh)

.PHONY: all
all: build build-devel build-oorthap


.PHONY: build
build:
	$(info Make: Building amd64 image.)
	docker-compose build --force-rm --build-arg BASE=ubuntu:18.04 \
	hackoort

.PHONY: build-devel
build-devel: build
	$(info Make: Building amd64 devel image.)
	docker-compose build --force-rm  hackoort-devel

.PHONY: build-oorthap
build-oorthap: build
	$(info Make: Building amd64 devel image.)
	docker-compose build --force-rm  oorthap


.PHONT: oorthap
oorthap: build-oorthap
	$(info Make: Starting containers.)
	docker-compose up -d oorthap


clean:
	-docker-compose down
	-docker image rm emsi/hackoort-devel
	-docker image rm emsi/oorthap
	-docker image rm emsi/hackoort


# raspberry targets

.PHONY: pi
pi: build-pi build-devel build-oorthap

.PHONY: build-pi
build-pi:
	$(info Make: Building raspberry image.)
	docker build --force-rm --build-arg BASE=raspbian/stretch \
	-t emsi/hackoort:raspberry .


# devel run/stop/enter targets

.PHONY: devel
devel: build-devel
	$(info Make: Starting containers.)
	docker-compose up -d hackoort-devel

.PHONY: stop-devel
stop-devel:
	$(info Make: Stopping environment containers.)
	docker-compose stop -d hackoort-devel

.PHONY:restart-devel
restart-devel:
	$(info Make: Restarting containers.)
	@make -s stop
	@make -s start

.PHONY:enter
enter:
	docker-compose exec hackoort-devel /bin/bash

