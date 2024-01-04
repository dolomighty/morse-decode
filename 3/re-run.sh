#!/bin/sh
exec xterm -geometry 130x30+0-0 -e "(make re ; make run) || read"
