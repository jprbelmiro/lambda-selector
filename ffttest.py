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
	linenp = np.asarray(line, dtype = "float32")
	planes = [np.asarray(linenp, dtype = "float32"), np.zeros(linenp.shape, dtype = "float32")]
	#Mat complexI;
	#complexI = cv2.merge(planes, 2)
	#complexI = cv2.dft(complexI, flags = cv2.DFT_ROWS)

	complex_cv = cv2.dft(linenp, flags = cv2.DFT_COMPLEX_OUTPUT)
	complex_scp = scipy.fft.fft(linenp)
	mag_scp = abs(complex_scp)
	print(sum(abs(mag_scp - result)))

#mag_test << mag(comples)

#diff(mag, mag_result)