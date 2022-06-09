from bdb import Bdb
from cProfile import label
from curses import pair_content
import os
import enum
import math
import xdrlib
import numpy as np
import matplotlib.pyplot as plt
from random import random 

### Parameters ###
# reference: https://www.usm.uni-muenchen.de/people/puls/lessons/numpraktnew/nbody/nbody_manual.pdf
G = 1.0         # Gravitational Constant
Md = 1.0        # Total mass of the disk
Mh = 5.8        # Total mass of the halo
Mb = 1/3        # Total mass of the bulge
h = 1.0         # Radial scale length
z0 = 0.2        # Characteristic scale height
gamma = 1.0     # Core radius
rc = 10         # Cutoff radius
a = 0.1         # Scale length

rmax = 10       # Maxium radius for sampling
Nd = 3000       # Number of disk particles
Nb = 1000       # Number of bulge particles
Nh = 6000       # Number of halo particles

eps = 1e-3      # Soften factor for forces calculation

filepath = "/cloud/NTHU/Researchs/G-SPH-C++-GPU/inputs/P10000New"   # save files location
    
# [L] = 3.5         kpc
# [T] = 1.31 x 10^7 yrs
# [M] = 5.6 x 10^10 solar mass
# [V] = 262         km/s
### ========= ###

class Particle():
    def __init__(self, x, y, z, vx, vy, vz, m, type) -> None:
        self.x = x
        self.y = y
        self.z = z
        self.vx = vx
        self.vy = vy
        self.vz = vz
        self.m = m
        self.type = type # disk, bulge, halo, blackhole
        self.r = np.sqrt(x**2 + y**2 + z**2)
        self.pos = np.array([x, y, z])
        self.vel = np.array([vx, vy, vz])
        self.force = np.array([0., 0., 0.])
        self.force_specie = np.array([0., 0., 0.]) # force caused by this specie (type) particles
        self.vel_specie = np.array([0., 0., 0.])

def diskDensity(x, y, z, Md, h, z0):
    R = np.sqrt(x**2+y**2)
    return (Md/(4*np.pi*(h**2)*z0))*np.exp(-R/h)/((np.cosh(z/z0))**2)

def haloDensity(x, y, z, Mh, rc, gamma):
    r = np.sqrt(x**2+y**2+z**2)
    q = gamma/rc
    alpha = 1/(1-np.sqrt(np.pi)*q*np.exp(q**2)*math.erfc(q))
    return (Mh/(2*(np.pi**1.5)))*(alpha/rc)*(np.exp(-r**2/rc**2)/(r**2+gamma**2))

def bulgeDensity(x, y, z, Mb, a):
    r = np.sqrt(x**2+y**2+z**2)
    return (Mb/(2*np.pi))*(a/r)/(r+a)**3

def generateDiskParticles(rmax, N, Md, h, z0):
    cnt = 0
    particles = []
    dv = (4*np.pi/3)*(rmax**3) / N
    rhomax = diskDensity(0, 0, 0, Md, h, z0) # to make sure prob. is always less than one
    while cnt < N:
        x = np.random.uniform(-rmax, rmax)
        y = np.random.uniform(-rmax, rmax)
        z = np.random.uniform(-rmax, rmax)
        r = np.sqrt(x**2+y**2+z**2)
        if r>rmax:
            continue
        rho = diskDensity(x, y, z, Md, h, z0)
        P = rho/rhomax # probability
        # assert(P<=1)
        if (P > 1):
            print(f"[generateDiskParticles]\tWarning: Probability > 1, please increase your N!")
        if (np.random.uniform(0, 1) <= P):
            cnt += 1
            particles.append([x,y,z])
            print(f"[generateDiskParticles]\tAdd {cnt} / {N} particles")
    return np.array(particles)

def generateBulgeParticles(rmax, N, Mb, a):
    cnt = 0
    particles = []
    dv = (4*np.pi/3)*(rmax**3) / N
    rhomax = bulgeDensity(0.05, 0, 0, Mb, a)
    while cnt < N:
        x = np.random.uniform(-rmax, rmax)
        y = np.random.uniform(-rmax, rmax)
        z = np.random.uniform(-rmax, rmax)
        r = np.sqrt(x**2+y**2+z**2)
        if r>rmax:
            continue
        rho = bulgeDensity(x, y, z, Mb, a)
        P = rho/rhomax # probability
        # assert(P<=1)
        if (P > 1):
            print(f"[generateBulgeParticles]\tWarning: Probability > 1, please increase your N!")
        if (np.random.uniform(0, 1) <= P):
            cnt += 1
            particles.append([x,y,z])
            print(f"[generateBulgeParticles]\tAdd {cnt} / {N} particles")
    return np.array(particles)

def generateHaloParticles(rmax, N, Mh, rc, gamma):
    cnt = 0
    particles = []
    dv = (4*np.pi/3)*(rmax**3) / N
    rhomax = haloDensity(0, 0, 0, Mh, rc, gamma)
    while cnt < N:
        # r = random()*(rmax-rmin) + rmin
        # phi = random()*2*np.pi
        # costheta = random()*2 - 1
        # theta = np.arccos(costheta)
        # x = r*np.sin(theta)*np.cos(phi)
        # y = r*np.sin(theta)*np.sin(phi)
        # z = r*np.cos(theta)
        x = np.random.uniform(-rmax, rmax)
        y = np.random.uniform(-rmax, rmax)
        z = np.random.uniform(-rmax, rmax)
        r = np.sqrt(x**2+y**2+z**2)
        if r>rmax:
            continue
        rho = haloDensity(x, y, z, Mh, rc, gamma)
        # P = ((rho/rhomax) * dv / Mh) # probability
        P = rho/rhomax
        assert(P<=1)
        if (P > 1):
            print(f"[generateHaloParticles]\tWarning: Probability > 1, please increase your N!")
        if (np.random.uniform(0, 1) <= P):
            cnt += 1
            particles.append([x,y,z])
            print(f"[generateHaloParticles]\tAdd {cnt} / {N} particles")
    return np.array(particles)

def saveParticles(rmax, Nd, Nb, Nh, Md, Mb, Mh, h, z0, a, rc, gamma, filepath):
    particles_d = generateDiskParticles(rmax, Nd, Md, h, z0)
    particles_b = generateBulgeParticles(rmax, Nb, Mb, a)
    particles_h = generateHaloParticles(rmax, Nh, Mh, rc, gamma)

    np.save(f"{filepath}/disk-N{Nd}", particles_d)
    np.save(f"{filepath}/bulge-N{Nb}", particles_b)
    np.save(f"{filepath}/halo-N{Nh}", particles_h)


    #fig = plt.figure(dpi=200)
    #ax = fig.gca(projection='3d')
    #ax.scatter(particles_d[:,0], particles_d[:,1], particles_d[:,2], s=0.1, c='blue')
    #ax.scatter(particles_b[:,0], particles_b[:,1], particles_b[:,2], s=0.1, c='red')
    #ax.scatter(particles_h[:,0], particles_h[:,1], particles_h[:,2], s=0.1, c='purple')
    ## plt.gca().set_aspect(1)
    #plt.show()
    return

def calculateVelocity(Pd, Pb, Ph, Nd, Nb, Nh, Md, Mb, Mh, G, eps):
    P = []
    md = Md/Nd
    mb = Mb/Nb
    mh = Mh/Nh
    for p in Pd:
        P.append(Particle(p[0], p[1], p[2], 0, 0, 0, md, 'disk'))
    for p in Pb:
        P.append(Particle(p[0], p[1], p[2], 0, 0, 0, mb, 'bulge'))
    for p in Ph:
        P.append(Particle(p[0], p[1], p[2], 0, 0, 0, mh, 'halo'))
    N = len(P)
    for i in range(N):
        print(f"[calculateVelocity]\tCalculate {i+1} / {N} particle's force")
        for j in range(i+1, N):
            r_vec = P[i].pos - P[j].pos
            r = np.linalg.norm(r_vec)
            P[i].force += -G*P[i].m*P[j].m * (r_vec/(r**2+eps**2)**1.5)
            P[j].force += G*P[i].m*P[j].m * (r_vec/(r**2+eps**2)**1.5)
            if (P[i].type == P[j].type):
                P[i].force_specie += -G*P[i].m*P[j].m * (r_vec/(r**2+eps**2)**1.5)
                P[j].force_specie += G*P[i].m*P[j].m * (r_vec/(r**2+eps**2)**1.5)

    for i in range(N):
        print(f"[calculateVelocity]\tCalculate {i+1} / {N} particle's velocity")
        force_to_center = np.dot(P[i].force, P[i].pos) * (P[i].pos/P[i].r**2)
        vc = np.sqrt(np.linalg.norm(force_to_center)*P[i].r/P[i].m)
        if (P[i].z > 0): # To make sure they are rotating along z-axis
            vc_unit_vector = np.cross(force_to_center, np.array([P[i].x, P[i].y, 0.0]))
        else:
            vc_unit_vector = -np.cross(force_to_center, np.array([P[i].x, P[i].y, 0.0]))        
        vc_unit_vector /= np.linalg.norm(vc_unit_vector)
        #if (np.linalg.norm(vc_unit_vector)!=1.0):
        #    print(vc_unit_vector)
        #    print(np.linalg.norm(vc_unit_vector))
        vc = vc * vc_unit_vector
        P[i].vel = vc
        P[i].vx = vc[0]
        P[i].vy = vc[1]
        P[i].vz = vc[2]
        #------------- specie -------------#
        force_to_center = np.dot(P[i].force_specie, P[i].pos) * (P[i].pos/P[i].r**2)
        vc = np.sqrt(np.linalg.norm(force_to_center)*P[i].r/P[i].m)
        vc_unit_vector = np.cross(force_to_center, np.array([P[i].x, P[i].y, 0.0]))
        vc_unit_vector /= np.linalg.norm(vc_unit_vector)
        vc = vc * vc_unit_vector
        P[i].vel_specie = vc
    return P

def toGSPHInput(particles_list, filepath, center=True):
    os.chdir(filepath)
    N = particles_list.size
    print(f"[toGSPHInput]\tRead {N} particles")
    if center:
        CM = np.array([0.,0.,0.])
        CM_vel = np.array([0.,0.,0.])
        for p in P:
            CM += p.pos
            CM_vel += p.vel
        CM /= N
        CM_vel /= N
        print(f"[toGSPHInput]\tCM: ({CM[0]}, {CM[1]}, {CM[2]})")
        print(f"[toGSPHInput]\tMean velocity: ({CM_vel[0]}, {CM_vel[1]}, {CM_vel[2]})")
        with open(f"P-N{N}.txt", "w+") as fh:
            fh.write(str(N)+"\n")
            for i,p in enumerate(P):
                fh.write(f"{i} {p.m} {p.x-CM[0]} {p.y-CM[1]} {p.z-CM[2]} {p.vx-CM_vel[0]} {p.vy-CM_vel[1]} {p.vz-CM_vel[2]} {0.00} {0.00} {0.00}\n")
    else:
        with open(f"P-N{N}.txt", "w+") as fh:
            fh.write(str(N)+"\n")
            for i,p in enumerate(P):
                fh.write(f"{i} {p.m} {p.x} {p.y} {p.z} {p.vx} {p.vy} {p.vz} {0.00} {0.00} {0.00}\n")
    return


if __name__ == '__main__':
    os.chdir(filepath)

    #saveParticles(rmax, Nd, Nb, Nh, Md, Mb, Mh, h, z0, a, rc, gamma, filepath)

    PD = np.load("disk-N3000.npy")
    PB = np.load("bulge-N1000.npy")
    PH = np.load("halo-N6000.npy")
    P = calculateVelocity(PD, PB, PH, Nd, Nb, Nh, Md, Mb, Mh, G, eps)
    np.save(f"{filepath}/P-N{Nd+Nb+Nh}", P)
    P = np.load(f"P-N10000.npy", allow_pickle=True)
    toGSPHInput(P, filepath)

    # P = np.load(f"P-N4000.npy", allow_pickle=True)
    # vc_d, vc_b, vc_h, vc = [], [], [], []
    # r_d, r_b, r_h, r = [], [], [], []
    # for p in P:
    #     if (p.type == 'disk'):
    #         r_d.append(p.r)
    #         vc_d.append(np.linalg.norm(p.vel_specie))
    #     elif (p.type == 'bulge'):
    #         r_b.append(p.r)
    #         vc_b.append(np.linalg.norm(p.vel_specie))
    #     elif (p.type == 'halo'):
    #         r_h.append(p.r)
    #         vc_h.append(np.linalg.norm(p.vel_specie))
    #     r.append(p.r)
    #     vc.append(np.linalg.norm(p.vel))
    
    # plt.figure(dpi=200)
    # plt.scatter(r, vc, s=1, c='black', alpha=0.8, label="Total")
    # plt.scatter(r_h, vc_h, s=1, c='purple', alpha=0.8, label="Halo")
    # plt.scatter(r_d, vc_d, s=1, c='blue', alpha=0.8, label="Disk")
    # plt.scatter(r_b, vc_b, s=1, c='red', alpha=0.8, label="Bulge")
    # plt.xlim(0,10)
    # plt.ylim(0,1)
    # plt.xlabel("r", fontsize=12)
    # plt.ylabel(r"$v_{c}$", fontsize=12)
    # plt.legend(fontsize=12)
    # plt.minorticks_on()
    # plt.show()


    # os.chdir("/Users/jamiechang917/Desktop")
    # PD = np.load("disk-N3000.npy")
    # PB = np.load("bulge-N1000.npy")
    # PH = np.load("halo-N6000.npy")
    # PD_r = np.linalg.norm(PD, axis=1)
    # PB_r = np.linalg.norm(PB, axis=1)
    # PH_r = np.linalg.norm(PH, axis=1)
    # DM, BM, HM = [],[],[]

    # R = np.linspace(0, 10, 123)
    # for r in R:
    #     dm = np.argwhere(PD_r <= r).size * Md/Nd
    #     bm = np.argwhere(PB_r <= r).size * Mb/Nb
    #     hm = np.argwhere(PH_r <= r).size * Mh/Nh
    #     DM.append(dm)
    #     BM.append(bm)
    #     HM.append(hm)
    # DM = np.array(DM)
    # BM = np.array(BM)
    # HM = np.array(HM)

    # plt.figure(dpi=200)
    # MM = DM+BM+HM
    # plt.scatter(R, DM, marker='x', c='r', alpha=0.5, s=10)
    # plt.scatter(R, BM, marker='x', c='g', alpha=0.5, s=10)
    # plt.scatter(R, HM, marker='x', c='blue', alpha=0.5, s=10)
    # plt.plot(R, DM, label="Disk\t($M_{total}=1.00$)", c='r')
    # plt.plot(R, BM, label="Bulge  ($M_{total}=0.33$)", c='g')
    # plt.plot(R, HM, label="Halo\t($M_{total}=5.80$)", c='blue')
    # plt.axhline(Md, color='grey', ls='--', alpha=0.5)
    # plt.axhline(Mb, color='grey', ls='--', alpha=0.5)
    # plt.legend()
    # plt.ylim(0,3)
    # # plt.gca().set_xscale('log')
    # # plt.gca().set_yscale('log')
    # plt.xlabel(r"$r$", fontsize=12)
    # plt.ylabel(r"$M(r)$", fontsize=12)
    # plt.gca().set_aspect(10/3)
    # plt.minorticks_on()
    # plt.show()



























    # R = np.linspace(0.04, 30, 100)
    # PHI = np.linspace(0, np.pi, 100)
    # THETA = np.linspace(0, 2*np.pi, 100)
    # DD = []
    # DM = []
    # BD = []
    # BM = []
    # HD = []
    # HM = []
    # for i,r in enumerate(R):
    #     dd, bd, hd = 0, 0, 0
    #     dm, bm, hm = 0, 0, 0
    #     dr, dv, ddv = 0, 0, 0
    #     if i > 0:
    #         dv = (4*np.pi/3)*(R[i]**3 - R[i-1]**3)
    #         ddv = dv/(PHI.size*THETA.size)
    #     print(r)
    #     for phi in PHI:
    #         for theta in THETA:
    #             x = r*np.sin(theta)*np.cos(phi)
    #             y = r*np.sin(theta)*np.sin(phi)
    #             z = r*np.cos(theta)
    #             dd += diskDensity(x, y, z, Md, h, z0)
    #             bd += bulgeDensity(x, y, z, Mb, a)
    #             hd += haloDensity(x, y, z, Mh, rc, gamma)
    #             dm += diskDensity(x, y, z, Md, h, z0) * ddv
    #             bm += bulgeDensity(x, y, z, Mb, a) * ddv
    #             hm += haloDensity(x, y, z, Mh, rc, gamma) * ddv 

    #     DD.append(dd/(PHI.size*THETA.size))
    #     BD.append(bd/(PHI.size*THETA.size))
    #     HD.append(hd/(PHI.size*THETA.size))
    #     if i == 0:
    #         DM.append(dm)
    #         BM.append(bm)
    #         HM.append(hm)
    #     else:
    #         DM.append(dm + DM[-1])
    #         BM.append(bm + BM[-1])
    #         HM.append(hm + HM[-1])
    # DM = np.array(DM)
    # BM = np.array(BM)
    # HM = np.array(HM)

    # Figure 1

    # plt.figure(dpi=200)
    # plt.plot(R, DD, label="Disk", c='r')
    # plt.plot(R, BD, label="Bulge", c='g')
    # plt.plot(R, HD, label="Halo", c='blue')
    # plt.legend()
    # plt.ylim(1e-8,1e2)
    # plt.gca().set_xscale('log')
    # plt.gca().set_yscale('log')
    # plt.xlabel(r"$r$", fontsize=12)
    # plt.ylabel(r"$\rho(r)$", fontsize=12)
    # plt.gca().set_aspect(1/3.3377)
    # plt.minorticks_on()
    # plt.show()

    # Figure 2, 3

    # plt.figure(dpi=200)
    # MM = DM+BM+HM
    # plt.plot(R, DM/MM, label="Disk", c='r')
    # plt.plot(R, BM/MM, label="Bulge", c='g')
    # plt.plot(R, HM/MM, label="Halo", c='blue')
    # plt.legend()
    # plt.ylim(0,1)
    # # plt.gca().set_xscale('log')
    # # plt.gca().set_yscale('log')
    # plt.xlabel(r"$r$", fontsize=12)
    # plt.ylabel(r"$M(r)/M_{total}$", fontsize=12)
    # plt.gca().set_aspect(5/1)
    # plt.minorticks_on()
    # plt.show()

    # Generate particles
    
    # particles_d = generateDiskParticles(rmax, Nd, Md, h, z0)
    # particles_b = generateBulgeParticles(rmax, Nb, Mb, a)
    # particles_h = generateHaloParticles(rmax, Nh, Mh, rc, gamma)


    # np.save(f"{filepath}/disk-N{Nd}", particles_d)
    # np.save(f"{filepath}/bulge-N{Nb}", particles_b)
    # np.save(f"{filepath}/halo-N{Nh}", particles_h)


    # fig = plt.figure(dpi=200)
    # ax = fig.gca(projection='3d')
    # ax.scatter(particles_d[:,0], particles_d[:,1], particles_d[:,2], s=0.1, c='blue')
    # ax.scatter(particles_b[:,0], particles_b[:,1], particles_b[:,2], s=0.1, c='red')
    # ax.scatter(particles_h[:,0], particles_h[:,1], particles_h[:,2], s=0.1, c='purple')
    # # plt.gca().set_aspect(1)
    # plt.show()






