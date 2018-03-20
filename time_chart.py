import sys

n_proc = [1,2,4,8,16]
n_thread = [1,2,4,8,16]

sys.stdout.write("np\\nth\t")
for t in n_thread:
    sys.stdout.write("%8d\t" % t)
sys.stdout.write("\n")

for p in n_proc:
    sys.stdout.write("%d\t" % p)
    max_t = 16/p
    for t in n_thread:
        if t > max_t:
            sys.stdout.write("--------\t")
            continue

        f = open('results/16384x4096_'+str(p)+'_'+str(t)+'.txt')
        S = 0.
        k = 0
        for line in f:
            S += float(line)
            k += 1
        sys.stdout.write("%lf\t" % (S/k))
        f.close()
    sys.stdout.write("\n")
