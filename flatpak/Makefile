.phony: build clean help install repo run test 

all: help

clean:
	-rm -rf build
	
build:
	flatpak-builder build org.nccmp.nccmp.json
  
help:
	@echo make [ build repo test install run clean ]

install:
	flatpak --user remote-add --no-gpg-verify nccmp-repo repo
	flatpak --user install nccmp-repo org.nccmp.nccmp

repo:
	flatpak-builder --repo=repo --force-clean build org.nccmp.nccmp.json
  
run:
	flatpak run org.nccmp.nccmp --help
  
test:
	flatpak-builder --run build org.nccmp.nccmp.json nccmp --help
  
  

