# G-SPH

> Work in Progress

## Introduction
This is an implemetaion of the gravitational SPH simulation written in C++. With Barnes-Hut algorithm and GPU acceleration, we are able to simulate the galaxies collision with many particles.

## Prerequisite

* g++
* Python 3 (need `numpy`)

### Install PyQT, PyOpenGL

See the guide in the official site.
* https://www.pyqtgraph.org/

```shell
$ pip install pyopengl pyqtgraph
```

### Install VisPy

* https://vispy.org/installation.html
```shell
$ pip install vispy
```
## Download and Compile
```shell
$ git clone https://github.com/jamiechang917/G-SPH-CPP
```
```shell
$ make
$ make clean
$ ./program
```
## Render
```
$ python render.py
```