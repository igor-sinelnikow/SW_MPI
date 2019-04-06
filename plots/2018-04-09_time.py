#
# MPI vs. MPI + OpenMP
# on BlueGene/P
#

folder = '../results/2018-04-09/'

len1 = '125678'
len2 = '4096'
size = len1+'x'+len2
threads = ['0','1','2','4']
n_proc = ['1','2','4','8','16','32','64','128','256','512']
times = []

for i in range(0,len(threads)):
    times.append([])
    for p in n_proc:
        f = open(folder+size+'_'+p+'_'+threads[i]+'.txt')
        S = 0.
        count = 0
        for line in f:
            S += float(line)
            count += 1
        if count == 0:
            times[i].append(0)
        else:
            times[i].append(S/count)
        f.close()

# print(times)

import numpy as np
import matplotlib.pyplot as plt

ind = np.arange(len(n_proc))
width = 0.22
labels = ['w/o OpenMP, only MPI','1 thread','2 threads','4 threads']

for i in range(0,len(threads)):
    plt.bar(ind+(i-3/2)*width,times[i],width,label=labels[i])

plt.title('BlueGene/P, '+size,size='large')
plt.ylabel('Time, sec')
plt.xlabel('Number of processors')
plt.xticks(ind,n_proc)

plt.legend(shadow=True)
plt.show()
