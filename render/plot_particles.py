from matplotlib.animation import FuncAnimation
import matplotlib.pyplot as plt
import sys
import os
import re
from tracemalloc import stop
import numpy as np
from pyqtgraph.Qt import QtCore, QtGui, QtWidgets
import pyqtgraph as pg
import pyqtgraph.opengl as gl

'''
This render is designed for rendering the particles from the outputs of G-SPH-C++.
'''

OUTPUT_DIR = r"/Users/jamiechang917/Desktop/output"
FPS = 30
pg.setConfigOptions(antialias=False, useOpenGL=True)
os.chdir(OUTPUT_DIR)

def readParticles(folder: str):
    '''
    Read particles of each step stored in input folder path.
    The format of the filename is partilces_{index}.txt
    '''
    os.chdir(folder)
    particles_history = {}
    KE_history = []
    PE_history = []
    TE_history = []
    all_particles_files = list(
        filter(lambda file: 'particles' in file, os.listdir(folder)))
    print("[Render] Reading particles...")
    for file in sorted(all_particles_files):
        index = int(re.findall("\d+", file)[0])
        with open(os.path.join(folder, file), 'r+') as fh:
            data = []
            lines = fh.readlines()
            info = np.array(lines[0].strip().split(" ")).astype(np.float64)
            N = int(info[0])  # The number of total partilces
            KE, PE, TE = info[1:4]
            KE_history.append(KE)
            PE_history.append(PE)
            TE_history.append(TE)
            # We should have N+1 lines in particles.txt
            assert len(lines) % (N+1) == 0
            for i in range(1, N+1):
                data.append(
                    np.array(lines[i].strip().split(" ")).astype(np.float64))
        particles_history[index] = np.array(data)
    return particles_history, KE_history, PE_history, TE_history

def makeAnimation(folder: str, fps=FPS):
    #############################################
    #                 Matplotlib                #
    #############################################
    os.chdir(folder)
    print("[Render] Make animation...")
    fig, ax = plt.subplots(dpi=200)
    data, KE, PE, TE = readParticles(folder=OUTPUT_DIR)
    xdata, ydata, zdata = data[0][:, 2], data[0][:, 3], data[0][:, 4]
    xdata1, ydata1, zdata1 = data[0][:3000, 2], data[0][:3000, 3], data[0][:3000, 4]
    xdata2, ydata2, zdata2 = data[0][3000:4000, 2], data[0][3000:4000, 3], data[0][3000:4000, 4]
    xdata3, ydata3, zdata3 = data[0][4000:, 2], data[0][4000:, 3], data[0][4000:, 4]
    
    plt.subplot(221)
    sca1_1 = plt.scatter(xdata1, ydata1, s=0.1, c="blue", alpha=0.5)
    sca1_2 = plt.scatter(xdata2, ydata2, s=0.1, c="red", alpha=0.5)
    sca1_3 = plt.scatter(xdata3, ydata3, s=0.05, c="purple", alpha=0.2)
    plt.gca().set_aspect(1)
    plt.xlim(-15, 15)
    plt.ylim(-15, 15)
    plt.xlabel("x")
    plt.ylabel("y")
    plt.gca().minorticks_on()

    plt.subplot(222)
    sca2_1 = plt.scatter(xdata1, zdata1, s=0.1, c="blue", alpha=0.5)
    sca2_2 = plt.scatter(xdata2, zdata2, s=0.1, c="red", alpha=0.5)
    sca2_3 = plt.scatter(xdata3, zdata3, s=0.05, c="purple", alpha=0.2)
    plt.gca().set_aspect(1)
    plt.xlim(-15, 15)
    plt.ylim(-15, 15)
    plt.xlabel("x")
    plt.ylabel("z")
    plt.gca().minorticks_on()

    plt.subplot(223)
    sca3_1 = plt.scatter(ydata1, zdata1, s=0.1, c="blue", alpha=0.5)
    sca3_2 = plt.scatter(ydata2, zdata2, s=0.1, c="red", alpha=0.5)
    sca3_3 = plt.scatter(ydata3, zdata3, s=0.05, c="purple", alpha=0.2)
    plt.gca().set_aspect(1)
    plt.xlim(-15, 15)
    plt.ylim(-15, 15)
    plt.xlabel("y")
    plt.ylabel("z")
    plt.gca().minorticks_on()

    ax4 = plt.subplot(224)
    ln1, = plt.plot([0],TE[0], c="coral", label="TE")
    ln2, = plt.plot([0],KE[0], c="red", label="KE")
    ln3, = plt.plot([0],PE[0], c="blue", label="PE")
    plt.legend()
    plt.xlabel("Iters")
    plt.ylabel("Energy")
    plt.gca().minorticks_on()
    plt.title("T = 0.00 Gyrs")

    plt.tight_layout()

    def update(frame):
        plt.title(f"T = {0.25*1.31e7*frame/1e9:.2f} Gyrs")
        sca1_1.set_offsets(data[frame][:3000, [2,3]])
        sca1_2.set_offsets(data[frame][3000:4000, [2,3]])
        sca1_3.set_offsets(data[frame][4000:, [2,3]])
        sca2_1.set_offsets(data[frame][:3000, [2,4]])
        sca2_2.set_offsets(data[frame][3000:4000, [2,4]])
        sca2_3.set_offsets(data[frame][4000:, [2,4]])
        sca3_1.set_offsets(data[frame][:3000, [3,4]])
        sca3_2.set_offsets(data[frame][3000:4000, [3,4]])
        sca3_3.set_offsets(data[frame][4000:, [3,4]])

        ln1.set_data(np.arange(frame+1), TE[:frame+1])
        ln2.set_data(np.arange(frame+1), KE[:frame+1])
        ln3.set_data(np.arange(frame+1), PE[:frame+1])
        ax4.set_ylim(min(PE[:frame+1]), max(KE[:frame+1]))
        ax4.set_xlim(0, frame+1)
        
        return sca1_1,sca1_2,sca1_3,sca2_1,sca2_2,sca2_3,sca3_1,sca3_2,sca3_3,ln1,ln2,ln3,

    ani = FuncAnimation(fig=fig, func=update, frames=range(max(data.keys())), interval=30)
    ani.save("ani.gif")
    plt.show()

    plt.clf()
    plt.plot(np.arange(len(KE)), KE)
    plt.plot(np.arange(len(PE)), PE)
    plt.plot(np.arange(len(TE)), TE)
    plt.show()


if __name__ == '__main__':
    makeAnimation(OUTPUT_DIR)
    

    # CM = []
    # f = []
    # for i in range(100):
    #     xdata, ydata, zdata = data[i][:, 2], data[i][:, 3], data[i][:, 4]
    #     CM.append([np.mean(xdata), np.mean(ydata), np.mean(zdata)])
    #     f.append(i)
    # CM = np.array(CM)
    # print(CM.shape)
    # plt.scatter(f, CM[:,0], s=1)
    # plt.show()
    


# class Visualizer(object):
#     def __init__(self, folder, box_size, frame) -> None:
#         self.data= readParticles(folder=folder)[0]
#         self.frame = frame
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
#         self.particles = gl.GLScatterPlotItem(pos=self.get_positions(0), pxMode=False, size=0.1, color=(100,100,100,1.0))
#         self.w.addItem(self.particles)


#         # cells grid
#         self.box_data = self._get_box_info(self.frame)
#         for box in self.box_data:

#             b = gl.GLBoxItem(size=QtGui.QVector3D(box[0],box[0],box[0]), color=(0,200,60,100))
#             b.translate(box[1], box[2], box[3])
#             self.w.addItem(b)
        
#         self.timer = QtCore.QTimer()
#         self.timer.timeout.connect(self.update())

#     def _get_box_info(self, frame):
#         with open(f"cells_{frame}.txt", "r+") as fh:
#             data = []
#             raw = fh.readlines()
#             for line in raw:
#                 data.append(np.array(line.strip().split(" ")).astype(np.float64))
#             return np.array(data)

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
#     v = Visualizer(OUTPUT_DIR, 40, 40)
#     v.animation()
