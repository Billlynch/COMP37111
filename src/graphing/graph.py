import numpy as np
import matplotlib
matplotlib.use('TkAgg')
from matplotlib import pyplot as plt
import csv
from scipy.optimize import curve_fit


frameDeltas = []
physicsDeltas = []
openGLDeltas = []
drawCallDeltas = []
particleCounts = []

rowCount = 5120
fifth = 1024

with open('analysisResultSpaceOpenCL.csv', newline='\n') as csvfile:
    analysisResultsReader = csv.reader(csvfile, delimiter=',')
    for row in analysisResultsReader:
        frameDeltas.append(row[1])
        physicsDeltas.append(row[2])
        openGLDeltas.append(row[3])
        drawCallDeltas.append(row[4])

with open('particleAtFrameCount.csv', newline='\n') as csvfile:
    frameCountParticleCountReader = csv.reader(csvfile, delimiter=',')
    for row in frameCountParticleCountReader:
        particleCounts.append(row[1])



# split into groups of 5 for averaging
frameDeltas = frameDeltas[10:rowCount+10]
physicsDeltas = physicsDeltas[10:rowCount+10]
openGLDeltas = openGLDeltas[10:rowCount+10]
drawCallDeltas = drawCallDeltas[10:rowCount+10]
particleCounts = particleCounts[10:rowCount+10]

# p = list(map(int, particleCounts))
#
# plt.title('Particle Count vs Frame')
# plt.ylabel('Number of particles')
# plt.xlabel('Number of Frames Elapsed')
# plt.plot(range(0, 5120), p)
# plt.show()

frameDeltas = np.array(frameDeltas[0:rowCount]).astype(np.float)
frd1 = np.mean(frameDeltas[0:fifth])
frd2 = np.mean(frameDeltas[fifth:(fifth * 2)])
frd3 = np.mean(frameDeltas[fifth * 2:fifth * 3])
frd4 = np.mean(frameDeltas[fifth * 3:fifth * 4])
frd5 = np.mean(frameDeltas[fifth * 4:fifth * 5])
frameDeltasAverages = (frd1, frd2, frd3, frd4, frd5)


physicsDeltas = np.array(physicsDeltas[0:rowCount]).astype(np.float)
phd1 = np.mean(physicsDeltas[0:fifth])
phd2 = np.mean(physicsDeltas[fifth:(fifth * 2)])
phd3 = np.mean(physicsDeltas[fifth * 2:fifth * 3])
phd4 = np.mean(physicsDeltas[fifth * 3:fifth * 4])
phd5 = np.mean(physicsDeltas[fifth * 4:fifth * 5])
physicsDeltasAverages = (phd1, phd2, phd3, phd4, phd5)

openGLDeltas = np.array(openGLDeltas[0:rowCount]).astype(np.float)
gld1 = np.mean(openGLDeltas[0:fifth])
gld2 = np.mean(openGLDeltas[fifth:(fifth * 2)])
gld3 = np.mean(openGLDeltas[fifth * 2:fifth * 3])
gld4 = np.mean(openGLDeltas[fifth * 3:fifth * 4])
gld5 = np.mean(openGLDeltas[fifth * 4:fifth * 5])
openGLDeltasAverages = (gld1, gld2, gld3, gld4, gld5)

drawCallDeltas = np.array(drawCallDeltas[0:rowCount]).astype(np.float)
dcd1 = np.mean(drawCallDeltas[0:fifth])
dcd2 = np.mean(drawCallDeltas[fifth:(fifth * 2)])
dcd3 = np.mean(drawCallDeltas[fifth * 2:fifth * 3])
dcd4 = np.mean(drawCallDeltas[fifth * 3:fifth * 4])
dcd5 = np.mean(drawCallDeltas[fifth * 4:fifth * 5])
drawCallDeltasAverages = (dcd1, dcd2, dcd3, dcd4, dcd5)


ind = np.arange(5)    # the x locations for the groups
width = 0.35       # the width of the bars: can also be len(x) sequence


y_offset = physicsDeltasAverages
pFrameDelta = plt.bar(ind, frameDeltasAverages, width)

pPhysicsDelta = plt.bar(ind + width, physicsDeltasAverages, width)
pOpenGlDelta = plt.bar(ind + width, openGLDeltasAverages, width, bottom=y_offset)
y_offset = np.sum([y_offset, openGLDeltasAverages], axis=0)
pdrawCallDelta = plt.bar(ind + width, drawCallDeltasAverages, width, bottom=y_offset)

plt.title('Average Frame Delta Breakdown')
plt.ylabel('Time (second)')
plt.xlabel('Frame Average Groupings')
plt.xticks(ind + (width/2), ('0-1024', '1024-2048', '2048-3072', '3072-4096', '4096-5120'))
plt.legend((pFrameDelta[0], pPhysicsDelta[0], pOpenGlDelta[0], pdrawCallDelta[0]), ('overall frame delta', 'physics delta', 'OpenGL buffer loading delta', 'draw call delta'))

plt.show()

FrameRate = [] #Frame rate at each frame
for fd in frameDeltas:
    FrameRate.append(1 / fd)

# list of lists
fRforPaticles = {}

for i in range(0, 5120):
    if particleCounts[i] in fRforPaticles:
        fRforPaticles[particleCounts[i]].append(FrameRate[i])
    else:
        fRforPaticles[particleCounts[i]] = [FrameRate[i]]

X = []
Y = []
for pc in sorted(fRforPaticles):
    Y.append(np.mean(fRforPaticles[pc]))
    X.append(pc)


X = np.array(list(map(float, X)))

plt.title('Average Frame Rate vs Particle Count')
plt.ylabel('Frame Rate (FPS)')
plt.xlabel('Number of Particles')
plt.plot(X, Y, '.')
plt.show()
