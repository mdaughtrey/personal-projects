#!/bin/bash

sudo launchctl unload -w /Library/LaunchDaemons/org.macports.dansguardian.plist
sudo launchctl load -w /Library/LaunchDaemons/org.macports.dansguardian.plist
