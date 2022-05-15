import sys, os, re
from tracemalloc import stop
import numpy as np
from pyqtgraph.Qt import QtCore, QtGui, QtWidgets
import pyqtgraph as pg
import pyqtgraph.opengl as gl

'''
This render is designed for rendering the particles from the outputs of G-SPH-C++.
'''

OUTPUT_DIR = r"Z:\NTHU\Researchs\G-SPH-C++\output-1652550921"
FPS = 30
pg.setConfigOptions(antialias=False, useOpenGL=True)

def read_particles(folder:str):
    '''
    Read particles of each step stored in input folder path.
    The format of the filename is partilces_{index}.txt
    '''
    particles_history = {}
    all_particles_files = list(filter(lambda file: 'particles' in file, os.listdir(folder)))
    print("[Render] Reading particles...")
    for file in all_particles_files:
        index = int(re.findall("\d+", file)[0])
        with open(os.path.join(folder, file), 'r+') as fh:
            data = []
            lines = fh.readlines()
            N = int(lines[0]) # The number of total partilces
            assert len(lines) % (N+1) == 0 # We should have N+1 lines in particles.txt
            for i in range(1, N+1):
                data.append(np.array(lines[i].strip().split(" ")).astype(np.float64))
        particles_history[index] = np.array(data)
    return particles_history

#############################################
#                 Matplotlib                #
#############################################
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
fig, ax = plt.subplots()
data = read_particles(folder=OUTPUT_DIR)
xdata, ydata = data[0][:, 2], data[0][:, 3]
sca = plt.scatter(xdata, ydata, s=1)
plt.gca().set_aspect(1)

def update(frame):
    xdata, ydata = data[frame][:, 2], data[frame][:, 3]
    sca.set_offsets(data[frame][:, 2:4])
    return sca,

if __name__ == '__main__':
    ani = FuncAnimation(fig=fig, func=update, frames=range(max(data.keys())))
    plt.show()


# class Visualizer(object):
#     def __init__(self, folder, box_size) -> None:
#         self.data = read_particles(folder=folder)
#         self.frame = 0
#         self.app = QtWidgets.QApplication(sys.argv)
#         self.w = gl.GLViewWidget()
#         self.w.setWindowTitle("G-SPH-C++")
#         self.w.setGeometry(100, 100, 800, 600)
#         self.w.show()
#         # grid
#         gz = gl.GLGridItem(size=QtGui.QVector3D(box_size,box_size,box_size))
#         gz.translate(0, 0, 0)
#         self.w.addItem(gz)
#         # axis lines
#         axis = gl.GLAxisItem(glOptions='additive', size=QtGui.QVector3D(box_size,box_size,box_size))
#         self.w.addItem(axis)
#         # particles
#         self.particles = gl.GLScatterPlotItem(pos=self.get_positions(0), pxMode=False, size=1.0, color=(100,100,100,1.0))
#         self.w.addItem(self.particles)

#         self.timer = QtCore.QTimer()
#         self.timer.timeout.connect(self.update())

#     def get_positions(self, frame):
#         return self.data[frame][:, 2:5]
    
#     def start(self):
#         QtWidgets.QApplication.instance().exec()

#     def update(self):
#         # self.particles.pos = np.ascontiguousarray(self.get_positions(frame), dtype=np.float32)
#         self.particles.setData(pos=self.get_positions(self.frame))
#         self.frame += 1
#         print("update")

#     def animation(self):
#         # timer = QtCore.QTimer()
#         # timer.timeout.connect(self.update())
#         self.timer.start(1000/FPS)
#         self.start()
       
        


# if __name__ == '__main__':
#     v = Visualizer(OUTPUT_DIR, 200)
#     v.animation()