import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks

fileName = "./output/t1_data"

x0Axis = []
x1Axis = []
baselineY0Axis = []
baselineY1Axis = []

def getBaselineSweep(f):
    baselineSubsweep0 = {}
    baselineSubsweep1 = {}
    distanceXAxis = [];
    amplitudeSweep0 = [];
    amplitudeSweep1 = []
    distance = ""
    amplitude = ""
    index = 0;
    subsweepIndex = 0
    with open(f) as file: 
        for x in file:
            chars = [*x]
            if chars[2] == 'd' or chars[0] == 'E':
                break
            if (chars[9].isdigit()):
                subsweepIndex = int(chars[9])
            if chars[0].isdigit():
                #print("passed check")
                for y in range(8):
                    distance += chars[y]
                if (float(distance) < 0.1):
                    distance = ""
                    amplitude = ""
                    continue
                #print(float(distance))
                if (subsweepIndex == 0):
                    baselineSubsweep0.setdefault(float(distance), [])
                if (subsweepIndex == 1):
                    baselineSubsweep1.setdefault(float(distance), [])
                i = x.rfind(']')
                i += 3
                while chars[i] != ',':
                    amplitude += chars[i]
                    i += 1
                print(amplitude)
                if (subsweepIndex == 0):
                    baselineSubsweep0[float(distance)].append(int(amplitude.strip()))
                if (subsweepIndex == 1):
                    baselineSubsweep1[float(distance)].append(int(amplitude.strip()))
                #amplitudeYAxis.append(int(amplitude.strip()))
                #index += 1
                distance = ""
                amplitude = ""
    f = fileName + "_plot.png"
    x0Axis = []
    x1Axis = []
    for key, value in baselineSubsweep0.items():
        x0Axis.append(key)
        baselineY0Axis.append(np.mean(value))

    for key, value in baselineSubsweep1.items():
            x1Axis.append(key)
            baselineY1Axis.append(np.mean(value))


def plotSweepsAvg(f):
    distance = ""
    amplitude = ""
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
                break
            if (chars[9].isdigit()):
                subsweepIndex = int(chars[9])
            if chars[0].isdigit():
                #print("passed check")
                for y in range(8):
                    distance += chars[y]
                if (float(distance) < 0.1):
                    distance = ""
                    amplitude = ""
                    continue
                #print(float(distance))
                if (subsweepIndex == 0):
                    subsweep0.setdefault(float(distance), [])
                if (subsweepIndex == 1):
                    subsweep1.setdefault(float(distance), [])
                #print(subsweep0.keys())
                    #print(distance)
                #distanceXAxis.append(float(distance.strip()))
                i = x.rfind(']')
                i += 3
                while chars[i] != ',':
                    amplitude += chars[i]
                    i += 1
                print(amplitude)
                if (subsweepIndex == 0):
                    subsweep0[float(distance)].append(int(amplitude.strip()))
                if (subsweepIndex == 1):
                    subsweep1[float(distance)].append(int(amplitude.strip()))
                #amplitudeYAxis.append(int(amplitude.strip()))
                #index += 1
                distance = ""
                amplitude = ""
    f = fileName + "_plot.png"
    x0Axis = []
    y0Axis = []
    x1Axis = []
    y1Axis = []
    for key, value in subsweep0.items():
        x0Axis.append(key)
        y0Axis.append(np.mean(value))

    for key, value in subsweep1.items():
            x1Axis.append(key)
            y1Axis.append(np.mean(value))

    y0Axis = getAdjustedAmplitudes(x0Axis, y0Axis)
    y1Axis = getAdjustedAmplitudes(x1Axis, y1Axis)

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

def getAdjustedAmplitudes(x, y): 
    newAmplitudes = []
    index = 0
    for yVal in y:
        newAmplitudes.append(yVal - baselineY0Axis[index])
        index += 1
    return newAmplitudes



plt.clf()
getBaselineSweep("./output/t1_data_empty_scene.txt")
plotSweepsAvg(fileName + ".txt")
#print("b: " + str(len(baselineY0Axis)))


