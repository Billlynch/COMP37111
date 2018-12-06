import numpy as np
import matplotlib
matplotlib.use('TkAgg')
from matplotlib import pyplot as plt
import csv


frameDeltas = []
physicsDeltas = []
openGLDeltas = []
drawCallDeltas = []
particleCounts = []

rowCount = 5120

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
particleCounts =  np.array(particleCounts[10:rowCount+10]).astype(np.int64)
frameDeltas = np.array(frameDeltas[0:rowCount]).astype(np.float)



plt.title('Average Frame Rate vs Particle Count')
plt.ylabel('Frame Rate (FPS)')
plt.xlabel('Number of Particles')
plt.plot(particleCounts, frameDeltas, '.')
plt.show()
