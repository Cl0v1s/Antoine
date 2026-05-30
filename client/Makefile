.PHONY: build 3ds-toolchain

all: build


TOOL ?= $(shell command -v docker >/dev/null 2>&1 && echo docker || command -v podman >/dev/null 2>&1 && echo podman)


IMAGE_NAME := 3ds-toolchain:latest
3ds-toolchain:
	@if ! $(TOOL) image inspect $(IMAGE_NAME) >/dev/null 2>&1; then \
		$(TOOL) build -t $(IMAGE_NAME) ./3ds; \
	fi

client.3dsx: 3ds-toolchain
	$(TOOL) run --rm -v $(CURDIR):/project -w /project 3ds-toolchain make -C 3ds

client.nds:
	$(TOOL) run --rm -v $(CURDIR):/project -w /project devkitpro/devkitarm make -C nds

build: client.nds client.3dsx