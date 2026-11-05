import matplotlib.pyplot as plt
import io

distanceXAxis = []
amplitudeYAxis = []
distance = ""
amplitude = ""
index = 0
with open("./data_outputs/IQ_data_collection_waving_hand_3_right_to_left.txt") as file:  
    for x in file:
        chars = [*x]
        if (chars[2] == 'd'):
            break
        if chars[0].isdigit():
            #print("passed check")
            for y in range(8):
                distance += chars[y]
                #print(distance)
            distanceXAxis.append(float(distance.strip()))
            i = x.rfind(', ')
            amplitude += chars[i + 2]
            amplitude += chars[i + 3]
            print(amplitude)
            amplitudeYAxis.append(int(amplitude.strip()))
            index += 1
            distance = ""
            amplitude = ""
            
            


for distance in distanceXAxis:
    print(distance)

for amplitude in amplitudeYAxis:
    print(distance)
    
plt.plot(distanceXAxis, amplitudeYAxis)
plt.xlabel("Distance (m)")
plt.ylabel("Amplitude")
plt.savefig("IQ_data_collection_waving_hand_3_right_to_left_graph.png", dpi=300)
plt.show()


with open("IQ_data_collection_waving_hand_3_right_to_left_DataX.txt", "w") as file:  
    for distance in distanceXAxis:
        file.write(f"{distance:.6f}\n")
with open("IQ_data_collection_waving_hand_3_right_to_left_DataY.txt", "w") as file:  
    for distance in amplitudeYAxis:
        file.write(str(distance) + "\n")


