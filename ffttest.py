import numpy as np
import csv
import scipy
import matplotlib.pyplot as plt
import cv2

#vector << read file
#mag_result << read file
#vector = pd.read_csv("line.csv", header = None)
vectors = []
mag_result = []

with open('line.csv', newline='\n') as csvfile:
		lineReader = csv.reader(csvfile, delimiter=',', quotechar='|')
		for row in lineReader: 
			vectors.append([float(i) for i in row])

with open('mag_result.csv', newline='\n') as csvfile:
		magReader = csv.reader(csvfile, delimiter=',', quotechar='|')
		for row in magReader: 
			mag_result.append([float(i) for i in row])


#complex << fft(vector)
for (line, result) in zip(vectors, mag_result):
	magcv = cv2.dft(line)
	mag = scipy.fft.fftshift(scipy.fft.fft(line))
	freq = scipy.fft.fftshift(scipy.fft.fftfreq(mag.size))
	print(mag- magcv)
	#print(len(mag)/np.argmax(mag))
	#plt.plot(freq, mag)
	#plt.waitforbuttonpress();
	#plt.show();

#mag_test << mag(comples)

#diff(mag, mag_result)