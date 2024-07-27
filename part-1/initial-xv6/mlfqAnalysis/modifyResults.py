
def getData(file):
    with open(file+"Results", "r") as f:
        data = f.readlines()
        for i in range(len(data)):
            data[i] = data[i].strip().split(',')
            for j in range(3):
                data[i][j] = eval(data[i][j])

        # normalize ticks
        timeStart = data[0][0]
        for i in range(len(data)):
            data[i][0] = data[i][0] - timeStart
        
        # remove init, sh and schedulertest (pid=1,2,3)
        # change pid from 8+ to 1+
        newData = []
        for i in range(len(data)):
            if data[i][1] >8:
                newData.append([data[i][0], data[i][1]-8, data[i][2], data[i][3]])
            # newData.append([data[i][0], data[i][1], data[i][2], data[i][3]])

        with open(file+"Data", "w") as p:
            for i in range(len(newData)):
                if i>0 and newData[i][0] != newData[i-1][0]:
                    p.write("\n");
                p.write(str(newData[i])+"\n")
        # print(newData)
        return newData

if __name__== "__main__":
    getData("mlfq");