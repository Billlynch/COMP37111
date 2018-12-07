import csv
import numpy as np


deltas = []
useDeltas = []

with open('analysisResult-950-Mac.csv', newline='\n') as csvfile:
    analysisResultsReader = csv.reader(csvfile, delimiter=',')
    for row in analysisResultsReader:
        deltas.append(row[1])

deltas = np.array(deltas).astype(np.float)

acc = 0
for d in deltas:
    acc += d
    useDeltas.append(d)
    if acc > 30:
        break



print(np.mean(useDeltas))
