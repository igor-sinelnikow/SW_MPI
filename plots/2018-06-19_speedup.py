#
# 1-20 processes & 1-8 threads
# on IBM Polus
#

folder = '../results/2018-06-19/'

len1 = '61845'
len2 = '34720'
size = len1+'x'+len2
threads = ['1','2','4','8']
n_proc = ['1','2','4','8','12','16','20']
speedup = []

f = open(folder+size+'_1_1.txt')
t1 = 0.
count = 0
for line in f:
    t1 += float(line)
    count += 1
t1 /= count
f.close()

for i in range(0,len(threads)):
    speedup.append([])
    for p in n_proc:
        f = open(folder+size+'_'+p+'_'+threads[i]+'.txt')
        S = 0.
        count = 0
        for line in f:
            S += float(line)
            count += 1
        S /= count
        speedup[i].append(t1/S)
        f.close()

print(speedup)

import numpy as np
import matplotlib.pyplot as plt

ind = np.arange(len(n_proc))
width = 0.22
labels = ['1 thread','2 threads','4 threads','8 threads']

for i in range(0,len(threads)):
    plt.bar(ind+(i-3/2)*width,speedup[i],width,label=labels[i])

plt.title('IBM Polus, '+size,size='large')
plt.ylabel('Speedup')
plt.xlabel('Number of processors')
plt.xticks(ind,n_proc)

plt.legend(shadow=True)
plt.show()
