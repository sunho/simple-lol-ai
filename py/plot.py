d=open("ad.txt","r")
lines=d.readlines()
import matplotlib.pyplot as plt
for line in lines:
	f=int(line)
	plt.plot(f)
plt.show()
print("ASD")
