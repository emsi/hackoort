PWD = $(shell pwd)
HACKOORT_TAG = $(shell ./pi.sh)
BASE=ubuntu:18.04

.PHONY: all
all: build build-devel build-oorthap


.PHONY: build
build:
	$(info Make: Building hackoort${HACKOORT_TAG} image.)
	HACKOORT_TAG=${HACKOORT_TAG} docker-compose build \
		--force-rm --build-arg BASE=$(BASE) hackoort

.PHONY: build-devel
build-devel: build
	$(info Make: Building hackoort-devel{HACKOORT_TAG} image.)
	HACKOORT_TAG=${HACKOORT_TAG}  docker-compose build --force-rm  hackoort-devel

.PHONY: build-oorthap
build-oorthap: build
	$(info Make: Building oorthap${HACKOORT_TAG} image.)
	HACKOORT_TAG=${HACKOORT_TAG} docker-compose build --force-rm  oorthap


.PHONT: oorthap
oorthap: build-oorthap
	$(info Make: Starting containers.)
	HACKOORT_TAG=${HACKOORT_TAG} docker-compose up -d oorthap


clean:
	-docker-compose down
	-docker image rm emsi/hackoort-devel
	-docker image rm emsi/oorthap
	-docker image rm emsi/hackoort

# devel run/stop/enter targets

.PHONY: devel
devel: build-devel
	$(info Make: Starting containers.)
	HACKOORT_TAG=${HACKOORT_TAG} docker-compose up -d hackoort-devel

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
	HACKOORT_TAG=${HACKOORT_TAG}  docker-compose exec hackoort-devel /bin/bash

