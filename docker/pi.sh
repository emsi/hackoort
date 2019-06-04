#!/bin/bash

OSRELEASE="/etc/os-release"
if [[ -f "$OSRELEASE" ]]; then
	if grep -q raspbian "$OSRELEASE"; then
	    echo :raspberry
	fi
fi
