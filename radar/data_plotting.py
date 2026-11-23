import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks
import math

fileName = "./output/t1_data"

x0Axis = []
x1Axis = []

def getBaselineSweep(f):
    subsweep0 = {}
    subsweep1 = {}
    distanceXAxis = [];
    amplitudeSweep0 = [];
    amplitudeSweep1 = []
    baselineSubsweep0 = {}
    baselineSubsweep1 = {}
    distance = ""
    IQVal = ""
    index = 0;
    subsweepIndex = 0
    with open(f) as file: 
        for x in file:
            chars = [*x]
            if chars[2] == 'd' or chars[0] == 'E':
                continue
            if (chars[9].isdigit()):
                subsweepIndex = int(chars[9])
            if chars[0].isdigit():
                #print("passed check")
                for y in range(8):
                    distance += chars[y]
                if (float(distance) < 0.1):
                    distance = ""
                    IQVal = ""
                    continue
                #print(float(distance))
                if (subsweepIndex == 0):
                    subsweep0.setdefault(float(distance), [])
                if (subsweepIndex == 1):
                    subsweep1.setdefault(float(distance), [])
                i = x.rfind('[')
                i += 1
                while chars[i] != ']':
                    IQVal += chars[i]
                    i += 1
                if (subsweepIndex == 0):
                    subsweep0[float(distance)].append(IQVal.strip())
                if (subsweepIndex == 1):
                    subsweep1[float(distance)].append(IQVal.strip())
                #amplitudeYAxis.append(int(amplitude.strip()))
                #index += 1
                distance = ""
                IQVal = ""
    f = fileName + "_plot.png"
    x0Axis = []
    x1Axis = []
    baselineSubsweep0 = getMeanIQValues(subsweep0)
    baselineSubsweep1 = getMeanIQValues(subsweep1)
    return baselineSubsweep0, baselineSubsweep1

def plotSweepsAvg(f, baselineSubsweep0, baselineSubsweep1):
    distance = ""
    IQVal = ""
    subsweep0 = {}
    subsweep1 = {}
    distanceXAxis = [];
    amplitudeSweep0 = [];
    amplitudeSweep1 = [];
    index = 0;
    subsweepIndex = 0
    with open(f) as file: 
        for x in file:
            chars = [*x]
            if chars[2] == 'd' or chars[0] == 'E':
                continue
            if (chars[9].isdigit()):
                subsweepIndex = int(chars[9])
            if chars[0].isdigit():
                #print("passed check")
                for y in range(8):
                    distance += chars[y]
                #print(float(distance))
                if float(distance) < 0.1:
                    distance = ""
                    IQVal = ""
                    continue
                if (subsweepIndex == 0):
                    subsweep0.setdefault(float(distance), [])
                if (subsweepIndex == 1):
                    subsweep1.setdefault(float(distance), [])
                #print(subsweep0.keys())
                    #print(distance)
                #distanceXAxis.append(float(distance.strip()))
                i = x.rfind('[')
                i += 1
                while chars[i] != ']':
                    IQVal += chars[i]
                    i += 1
                if (subsweepIndex == 0):
                    subsweep0[float(distance)].append(IQVal.strip())
                if (subsweepIndex == 1):
                    subsweep1[float(distance)].append(IQVal.strip())
                #amplitudeYAxis.append(int(amplitude.strip()))
                #index += 1
                distance = ""
                IQVal = ""
    f = fileName + "_plot.png"
    x0Axis = []
    y0Axis = []
    x1Axis = []
    y1Axis = []
    newSubsweep0 = getMeanIQValues(subsweep0)
    newSubsweep1 = getMeanIQValues(subsweep1)
    print(subsweep0.items())
    for key, value in subsweep0.items():
        x0Axis.append(key)
        newITerm = newSubsweep0[key][0] - baselineSubsweep0[key][0]
        newQTerm = newSubsweep0[key][1] - baselineSubsweep0[key][1]
        amplitude = math.sqrt((newITerm ** 2) + (newQTerm ** 2));
        y0Axis.append(amplitude)

    for key, value in subsweep1.items():
        x1Axis.append(key)
        newITerm = newSubsweep1[key][0] - baselineSubsweep1[key][0]
        newQTerm = newSubsweep1[key][1] - baselineSubsweep1[key][1]
        amplitude = math.sqrt((newITerm ** 2) + (newQTerm ** 2));
        y1Axis.append(amplitude)    

    # y0Axis = getAdjustedAmplitudes(x0Axis, y0Axis)
    # y1Axis = getAdjustedAmplitudes(x1Axis, y1Axis)

    plt.figure(1)
    fig, axs = plt.subplots(2, 1, figsize=(10, 6))
    maxAmplitude = np.max(y0Axis);
    axs[0].set_yticks(np.arange(0, maxAmplitude + 20, 10))
    axs[0].set_xticks(np.arange(0, 1.5, 0.1))
    points = find_peaks(y0Axis, height = 50)
    for i in points[0]: 
        axs[0].plot(x0Axis[i], y0Axis[i], "x", color="red")
        axs[0].annotate(
        "(" + str((f"{x0Axis[i]:.2f}")) + "," + str((f"{y0Axis[i]:.0f}")) + ")", 
        xy=(x0Axis[i], y0Axis[i]), 
        xytext=(5, -5), 
        textcoords="offset points"
        )
    axs[0].plot(x0Axis, y0Axis)
    axs[0].set_title("Mean Subsweep 0")
    axs[0].set_xlabel("Distance (cm)")
    axs[0].set_ylabel("Amplitude")
    maxAmplitude = np.max(y1Axis);
    axs[1].set_yticks(np.arange(0, maxAmplitude + 50, 25))
    axs[1].set_xticks(np.arange(0, 1.5, 0.1)) 
    axs[1].plot(x1Axis, y1Axis, color="orange")
    axs[1].set_title("Mean Subsweep 1")
    axs[1].set_xlabel("Distance (m)")
    axs[1].set_ylabel("Amplitude")
    points = find_peaks(y1Axis, height = 50)
    for i in points[0]: 
        axs[1].plot(x1Axis[i], y1Axis[i], "x", color="red")
        axs[1].annotate(
        "(" + str((f"{x1Axis[i]:.2f}")) + "," + str((f"{y1Axis[i]:.0f}")) + ")", 
        xy=(x1Axis[i], y1Axis[i]), 
        xytext=(5, -5), 
        textcoords="offset points"
        )
    plt.tight_layout()
    plt.savefig(f, dpi=300)

# def getAdjustedAmplitudes(x, y): 
#     newAmplitudes = []
#     index = 0
#     for yVal in y:
#         newAmplitudes.append(yVal - baselineY0Axis[index])
#         index += 1
#     return newAmplitudes

def getMeanIQValues(subsweep):
    iTerm = 0
    QTerm = 0
    TermCounter = 0
    temp = {}
    for key, value in subsweep.items():
        temp.setdefault(key, [])
        for IQVal in value:
            if (IQVal.rfind("+") == -1):
                i = IQVal.rfind("-")
                iTerm += int(IQVal[0:i])
                TermCounter += 1
                #print(IQVal[0:i])
            else:
                i = IQVal.rfind("+")
                TermCounter += 1;  
                iTerm += int(IQVal[0:i])   
                #print(IQVal[0: i])   
            QTerm += int(IQVal[i: len(IQVal) - 1])
        temp[key].append(iTerm / TermCounter)
        temp[key].append(QTerm / TermCounter)
        TermCounter = 0
        QTerm = 0
        iTerm = 0    
    #print(temp.items())
    return temp
plt.clf()
base0, base1 = getBaselineSweep("./output/t1_data_empty_scene.txt")
plotSweepsAvg(fileName + ".txt",  base0, base1)
#print("b: " + str(len(baselineY0Axis)))

