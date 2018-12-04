import numpy as np
import matplotlib.pyplot as plt
import csv

frameDeltas = []
physicsDeltas = []
openGLDeltas = []
drawCallDeltas = []

rowCount = 5120
fith = 1024

with open('../../cmake-build-debug/analysisResult.csv', newline='\n') as csvfile:
    analysisResultsReader = csv.reader(csvfile, delimiter=',')
    for row in analysisResultsReader:
        frameDeltas.append(row[1])
        physicsDeltas.append(row[2])
        openGLDeltas.append(row[3])
        drawCallDeltas.append(row[4])



# split into groups of 5 for averaging
frameDeltas = frameDeltas[10:rowCount+10]
physicsDeltas = physicsDeltas[10:rowCount+10]
openGLDeltas = openGLDeltas[10:rowCount+10]
drawCallDeltas = drawCallDeltas[10:rowCount+10]


frameDeltas = np.array(frameDeltas[0:rowCount]).astype(np.float)
frd1 = np.mean(frameDeltas[0:fith])
frd2 = np.mean(frameDeltas[fith:(fith * 2)])
frd3 = np.mean(frameDeltas[fith * 2:fith * 3])
frd4 = np.mean(frameDeltas[fith * 3:fith * 4])
frd5 = np.mean(frameDeltas[fith * 4:fith * 5])
frameDeltasAverages = (frd1, frd2, frd3, frd4, frd5)


physicsDeltas = np.array(physicsDeltas[0:rowCount]).astype(np.float)
phd1 = np.mean(physicsDeltas[0:fith])
phd2 = np.mean(physicsDeltas[fith:(fith * 2)])
phd3 = np.mean(physicsDeltas[fith * 2:fith * 3])
phd4 = np.mean(physicsDeltas[fith * 3:fith * 4])
phd5 = np.mean(physicsDeltas[fith * 4:fith * 5])
physicsDeltasAverages = (phd1, phd2, phd3, phd4, phd5)

openGLDeltas = np.array(openGLDeltas[0:rowCount]).astype(np.float)
gld1 = np.mean(openGLDeltas[0:fith])
gld2 = np.mean(openGLDeltas[fith:(fith * 2)])
gld3 = np.mean(openGLDeltas[fith * 2:fith * 3])
gld4 = np.mean(openGLDeltas[fith * 3:fith * 4])
gld5 = np.mean(openGLDeltas[fith * 4:fith * 5])
openGLDeltasAverages = (gld1, gld2, gld3, gld4, gld5)

drawCallDeltas = np.array(drawCallDeltas[0:rowCount]).astype(np.float)
dcd1 = np.mean(drawCallDeltas[0:fith])
dcd2 = np.mean(drawCallDeltas[fith:(fith * 2)])
dcd3 = np.mean(drawCallDeltas[fith * 2:fith * 3])
dcd4 = np.mean(drawCallDeltas[fith * 3:fith * 4])
dcd5 = np.mean(drawCallDeltas[fith * 4:fith * 5])
drawCallDeltasAverages = (dcd1, dcd2, dcd3, dcd4, dcd5)


ind = np.arange(5)    # the x locations for the groups
width = 0.35       # the width of the bars: can also be len(x) sequence


y_offset = physicsDeltasAverages

pFrameDelta = plt.bar(ind, frameDeltasAverages, width)
pPhysicsDelta = plt.bar(ind + width, physicsDeltasAverages, width)
pOpenGlDelta = plt.bar(ind + width, openGLDeltasAverages, width, bottom=y_offset)
y_offset = np.sum([y_offset, openGLDeltasAverages], axis=0)
pdrawCallDelta = plt.bar(ind + width, drawCallDeltasAverages, width, bottom=y_offset)

plt.ylabel('time in ms')
plt.title('deltas for bottle necks')
plt.xticks(ind + (width/2), ('0-1024', '1024-2048', '2048-4096', '4096-5120', '??'))
plt.yticks(np.arange(0, 0.03, 0.001))
plt.legend((pFrameDelta[0], pPhysicsDelta[0], pOpenGlDelta[0], pdrawCallDelta[0]), ('overall frame delta', 'physics delta', 'OpenGL buffer loading delta', 'draw call delta'))

plt.show()