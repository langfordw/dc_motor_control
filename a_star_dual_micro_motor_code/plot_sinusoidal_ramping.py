import matplotlib
from matplotlib import pyplot as plt
import numpy as np

data = np.loadtxt('sinusoidal_ramping6.csv', delimiter=',')
# data = np.loadtxt('linear_ramping4.csv',delimiter=',')

# t = data[:,1]-data[0,0]
# sin2x = 1900*np.sin(t/750.*1.571+1.571)**2
# # dx = 1900-1900*0.5*(1-np.cos(2*(t/750.*1.571)))
# frac = t/750
# dx = 1900-(3-2*frac)*frac*frac*1900

# plt.plot(data[:,0],'g')
# plt.plot(data[:,0]-data[0,0],data[:,2],'r')
plt.plot(data[:,1]-data[0,0],data[:,1],'g')
plt.plot(data[:,1]-data[0,0],data[:,2],'r')
# plt.plot(t,dx,'b')
# plt.plot(t,sin2x,'r')
# plt.plot(t,dxx,'k')
plt.xlim([0,1600])
plt.ylim([0,2000])
# plt.legend(['sin(x)^2','-3t^3+2t^2'])
plt.show()

