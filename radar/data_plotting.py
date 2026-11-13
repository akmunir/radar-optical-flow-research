import matplotlib.pyplot as plt
import numpy as np


plt.xlabel("Distance (cm)")
plt.ylabel("Amplitude")

def plotSubSweeps(f):
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
            if (chars[0] == 'S'):
                subsweepIndex += 1
                # plt.plot(distanceXAxis, amplitudeYAxis)
                plt.plot(distanceXAxis, amplitudeYAxis, label="Subsweep " + str(subsweepIndex))
                distanceXAxis = []
                amplitudeYAxis = []
            if chars[0].isdigit():
                #print("passed check")
                for y in range(8):
                    distance += chars[y]
                    #print(distance)
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
                
            
plotSubSweeps("./IQ_data_collection_initial_stationary_pendulum_measurement.txt");
f = "./IQ_data_collection_initial_stationary_pendulum_measurement.png"
plt.legend()
plt.title("Single Sweep")
plt.xlim(0, 0.6)
plt.xticks(np.arange(0, 0.61, 0.05))
plt.savefig(f, dpi=300)

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



