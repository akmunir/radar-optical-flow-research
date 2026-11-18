import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks

fileName = "./output/t5_raw_data"

def plotSingleSweep(f):
    distance = ""
    amplitude = ""
    distanceXAxis = [];
    amplitudeYAxis = [];
    index = 0;
    subsweepIndex = 0;
    with open(f) as file: 
        for x in file:
            chars = [*x]
            if chars[2] == 'd' or chars[0] == 'E':
                break
            if (chars[0] == 'B'):
                print("new sweep")
            if (chars[0] == 's'):
                subsweepIndex += 1
                #plt.plot(distanceXAxis, amplitudeYAxis)
                # print(distanceXAxis)
                # print(amplitudeYAxis)
                plt.plot(distanceXAxis, amplitudeYAxis, label="Subsweep " + str(subsweepIndex))
                distanceXAxis = []
                amplitudeYAxis = []
            if chars[0].isdigit():
                #print("passed check")
                for y in range(8):
                    distance += chars[y]
                    #print(distance)
                if (float(distance) < 0.1):
                    distance = ""
                    amplitude = ""
                    continue
                distanceXAxis.append(float(distance.strip()))
                i = x.rfind(']')
                i += 3
                while chars[i] != ',':
                    amplitude += chars[i]
                    i += 1
                print(amplitude)
                amplitudeYAxis.append(int(amplitude.strip()))
                index += 1
                distance = ""
                amplitude = ""
    plt.xlabel("Distance (cm)")
    plt.ylabel("Amplitude")
    f = "./test2_plot.png"
    plt.legend()
    #plt.plot(distanceXAxis, amplitudeYAxis)
    plt.title("Single Sweep")
    plt.xlim(0, 1.0)
    plt.xticks(np.arange(0, 1.0, 0.05))
    plt.savefig(f, dpi=300)

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

    plt.figure(1)
    fig, axs = plt.subplots(2, 1, figsize=(10, 6))
    maxAmplitude = np.max(y0Axis);
    axs[0].set_yticks(np.arange(0, maxAmplitude + 20, 10))
    axs[0].set_xticks(np.arange(0, 1.5, 0.1))
    points = find_peaks(y0Axis, height = 25)
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
    axs[1].set_xlabel("Distance (cm)")
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
## 33.6 cm

#plotSingleSweep("./test2.txt");        
#plotSingleSweep("./IQ_data_collection_initial_stationary_pendulum_measurement.txt");
plt.clf()
plotSweepsAvg(fileName + ".txt")
# distance from bob to pendulum is roughly 34.3 cm

# for distance in distanceXAxis:
#     print(distance)

# for amplitude in amplitudeYAxis:
#     print(distance)
    
# plt.plot(distanceXAxis, amplitudeYAxis)
# plt.xlabel("Distance (m)")
# plt.ylabel("Amplitude")
# plt.savefig("IQ_data_collection_waving_hand_3_right_to_left_graph.png", dpi=300)
# plt.show()


# with open("IQ_data_collection_waving_hand_3_right_to_left_DataX.txt", "w") as file:  
#     for distance in distanceXAxis:
#         file.write(f"{distance:.6f}\n")
# with open("IQ_data_collection_waving_hand_3_right_to_left_DataY.txt", "w") as file:  
#     for distance in amplitudeYAxis:
#         file.write(str(distance) + "\n")



