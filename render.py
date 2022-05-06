from matplotlib.pyplot import box, draw
from pyqtgraph.Qt import QtCore, QtGui, QtWidgets
import pyqtgraph as pg
import pyqtgraph.opengl as gl
import numpy as np
import sys

BOX_SIZE = 200
pg.setConfigOptions(antialias=False, useOpenGL=True)

def read_data(filepath:str):
    print(f"Reading {filepath}...")
    data = dict()
    with open(filepath, "r+") as fh:
        raw = fh.readlines()
        N = int(raw[0]) # we have (N+1) times of lines
        assert len(raw) % (N+1) == 0
        total_frames = len(raw) // (N+1)
        print(N, total_frames)
        for frame in range(total_frames):
            data_section = (frame*(N+1), (frame+1)*(N+1))
            this_frame_N = raw[data_section[0]]
            this_frame_data = []
            for i in range(data_section[0]+1, data_section[1]):
                p = np.array(raw[i].strip().split(" ")).astype(np.float64)
                this_frame_data.append(p)        
            data[frame] = np.array(this_frame_data)
    return data


class Visualizer(object):
    def __init__(self, data_file:str) -> None:
        self.data = read_data(data_file)
        
        self.app = QtWidgets.QApplication(sys.argv)
        self.w = gl.GLViewWidget()
        self.w.setWindowTitle("G-SPH-C++")
        self.w.setGeometry(100, 100, 800, 600)
        self.w.show()

        # gx = gl.GLGridItem()
        # gx.rotate(90, 0, 1, 0)
        # gx.translate(0, 0, 0)
        # self.w.addItem(gx)
        # gy = gl.GLGridItem()
        # gy.rotate(90, 1, 0, 0)
        # gy.translate(0, 0, 0)
        # self.w.addItem(gy)
        gz = gl.GLGridItem(size=QtGui.QVector3D(BOX_SIZE, BOX_SIZE, BOX_SIZE))
        gz.translate(0, 0, 0)
        # self.w.addItem(gz)

        ax = gl.GLAxisItem(glOptions='additive')
        ax.setSize(BOX_SIZE,BOX_SIZE,BOX_SIZE)
        self.w.addItem(ax)

        self.particles = gl.GLScatterPlotItem(pos=self.get_particles_postion(0), pxMode=False, size=1.0, color=(100,100,100,1.0))
        self.w.addItem(self.particles)


        self.box_data = self._get_box_info()
        for box in self.box_data:
            b = gl.GLBoxItem(size=QtGui.QVector3D(box[0],box[0],box[0]), color=(0,200,60,80))
            b.translate(box[1], box[2], box[3])
            self.w.addItem(b)
        
        
        
    def _get_box_info(self):
        with open("cells.txt", "r+") as fh:
            data = []
            raw = fh.readlines()
            for line in raw:
                data.append(np.array(line.strip().split(" ")).astype(np.float64))
            return np.array(data)

    def get_particles_postion(self, frame):
        return self.data[frame][:, 2:5]

    def start(self):
        QtWidgets.QApplication.instance().exec()
        print(self.w.cameraPosition())


if __name__ == '__main__':
    v = Visualizer("particles.txt")
    v.start()
