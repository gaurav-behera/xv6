import matplotlib.pyplot as plt
from modifyResults import getData

class TickInfo:
    def __init__(self, id):
        self.id = id
        self.ticks = list()
        self.runningProcesses = list()
        self.sp = list()
        self.dp = list()
        self.rbi = list()

    def addEntry(self, tick, process, sp, dp, rbi, cpu):
        self.ticks.append(tick)
        self.runningProcesses.append(process)
        self.sp.append(sp)
        self.dp.append(dp)
        self.rbi.append(rbi)

class ProcessInfo:
    def __init__(self, pid):
        self.pid = pid
        self.ticks = list()
        self.states = list()
        self.sp = list()
        self.dp = list()
        self.rbi = list()

    def addEntry(self, tick, state, sp, dp, rbi):
        self.ticks.append(tick)
        self.states.append(state)
        self.sp.append(sp)
        self.dp.append(dp)
        self.rbi.append(rbi)

data = getData("pbs")
numOfProcesses = max([entry[1] for entry in data])
processes = [ProcessInfo(i + 1) for i in range(numOfProcesses)]
numOfCpus = max([entry[-2] for entry in data])+1
ticks = [TickInfo(numOfCpus) for i in range(numOfCpus)]

for entry in data:
    if entry[-1] == 'run' and entry[-2] > -1:
        ticks[entry[-2]].addEntry(entry[0], entry[1], entry[2], entry[3], entry[4], entry[5])
    processes[entry[1] - 1].addEntry(entry[0], entry[-1], entry[2], entry[3], entry[4])

fig, axs = plt.subplots(2, 2, figsize=(15, 8))

axs[0][0].set_title('Static Priority vs Ticks')
axs[0][0].set_xlabel("Number of ticks")
axs[0][0].set_ylabel("Static Priority")

axs[0][1].set_title('Dynamic Priority vs Ticks')
axs[0][1].set_xlabel("Number of ticks")
axs[0][1].set_ylabel("Dynamic Priority")

axs[1][0].set_title('RBI vs Ticks')
axs[1][0].set_xlabel("Number of ticks")
axs[1][0].set_ylabel("RBI")

axs[1][1].set_title('Current Running Process vs Ticks (on each CPU)')
axs[1][1].set_xlabel("Number of ticks")
axs[1][1].set_ylabel("Process ID")

colorDict = {1: '#1f77b4', 2: '#ff7f0e', 3: '#2ca02c', 4: '#d62728', 5: '#9467bd', 6: '#8c564b', 7: '#e377c2', 8: '#7f7f7f', 9: '#bcbd22', 10: '#17becf'}

def plotProcess(pid):
    xVals = processes[pid - 1].ticks
    yVals_sp = processes[pid - 1].sp
    yVals_dp = processes[pid - 1].dp
    yVals_rbi = processes[pid - 1].rbi

    axs[0][0].plot(xVals, yVals_sp, label=f"P{pid}", color=colorDict[pid])
    axs[0][1].plot(xVals, yVals_dp, label=f"P{pid}", color=colorDict[pid])
    axs[1][0].plot(xVals, yVals_rbi, label=f"P{pid}", color=colorDict[pid])

for pid in range(numOfProcesses):
    plotProcess(pid + 1)


for cpu in range(numOfCpus):
    xVals = ticks[cpu].ticks
    yVals = ticks[cpu].runningProcesses
    axs[1][1].plot(xVals, yVals, label=f"CPU {cpu + 1}", color=colorDict[cpu+1])

axs[0][0].legend()
axs[0][1].legend()
axs[1][0].legend()
axs[1][1].legend()
plt.tight_layout()
plt.show()
