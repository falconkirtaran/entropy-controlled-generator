# Makefile for entropy-controlled-generator

CFLAGS=-Wall -ggdb -O2
LDFLAGS=

all: t_control

t_control: t_control.cpp
	g++ t_control.cpp -o t_control $(CFLAGS) $(LDFLAGS)
