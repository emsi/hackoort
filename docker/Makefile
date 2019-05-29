PWD = $(shell pwd)

.PHONY: all
all: build start


.PHONY: build
build:
	$(info Make: Building amd64 image.)
	docker build --force-rm --build-arg BASE=ubuntu:18.04 \
	-t emsi/hackoort .

.PHONY: build-pi
build-gpu:
	$(info Make: Building raspberry image.)
	docker build --force-rm --build-arg BASE=raspbian/stretch \
	-t emsi/hackoort:raspberry .

.PHONY: start
start:
	$(info Make: Starting containers.)
	docker-compose up -d

.PHONY: stop
stop:
	$(info Make: Stopping environment containers.)
	docker-compose stop

.PHONY:restart
restart:
	$(info Make: Restarting containers.)
	@make -s stop
	@make -s start

.PHONY:enter
enter:
	docker-compose exec hackoort /bin/bash

