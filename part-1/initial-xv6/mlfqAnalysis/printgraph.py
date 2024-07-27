import matplotlib.pyplot as plt
from modifyResults import getData

class tickInfo:
    def __init__(self):
        self.ticks = list()
        self.runningProcess = list()
        self.queue = list()

    def addEntry(self, tick, process, queue):
        self.ticks.append(tick)
        self.runningProcess.append(process)
        self.queue.append(queue)

class processInfo:
    def __init__(self, pid):
        self.pid = pid
        self.ticks = list()
        self.queues = list()
        self.states = list()

    def addEntry(self, tick, queue, state):
        self.ticks.append(tick)
        self.queues.append(queue)
        self.states.append(state)


data = getData("mlfq")
numOfProcesses = max([entry[1] for entry in data])
processes = list()

for i in range(numOfProcesses):
    processes.append(processInfo(i+1))
ticks = tickInfo()

for entry in data:
    if entry[3] == 'run':
        ticks.addEntry(entry[0], entry[1], entry[2])
    processes[entry[1]-1].addEntry(entry[0], entry[2], entry[3])

plt.xlabel("Number of ticks")
plt.ylabel("Queue Number")
plt.title("Aging time: 30 ticks")

colorDict= {1:'#1f77b4', 2:'#ff7f0e', 3:'#2ca02c', 4:'#d62728', 5:'#9467bd'}

def plotProcess(pid):
    xVals = processes[pid-1].ticks
    yVals = processes[pid-1].queues
    plt.plot(xVals, yVals, label=f"P{pid}", color=colorDict[pid])
    # plt.plot(xVals, yVals, label=f"P{pid}")

def showProcessTimeline():
    for i in range(1,len(ticks.ticks)):
        xVals = [ticks.ticks[i-1], ticks.ticks[i]]
        yVals = [ticks.queue[i-1], ticks.queue[i]]
        if yVals[0] == yVals[1]:
            plt.plot(xVals, yVals, color=colorDict[ticks.runningProcess[i-1]])

for pid in range(numOfProcesses):
    plotProcess(pid+1)

# showProcessTimeline()

plt.legend()
plt.show()