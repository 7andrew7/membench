import matplotlib.pyplot as plt
import csv

x = []
y = []

with open('out.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    next(plots)
    
    for row in plots:
        x.append(float(row[10]))
        y.append(float(row[5]) / 10 **9)

plt.plot(x,y)
plt.xlabel('buffer size')
plt.ylabel('memory bandwidth (GB)')
plt.title('Memory bandwidth')
plt.xscale('log', basex=2)
plt.ylim(ymin=0)
plt.show()
