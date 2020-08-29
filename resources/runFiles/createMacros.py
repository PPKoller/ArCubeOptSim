import shutil
import numpy as np

#read voxel table
vox_tab = open('OptSim_LUT_voxel_table.txt', 'r')

cmin = np.array(vox_tab.readline().split()).astype(np.float)
cmax = np.array(vox_tab.readline().split()).astype(np.float)
vdim = np.array(vox_tab.readline().split()).astype(np.float)
nvox = np.array(vox_tab.readline().split()).astype(np.int)

print 'Number of voxels: %d' % np.prod(nvox)

#for vox in range(np.prod(nvox)):
for vox in range(10):

    if (vox%1000 == 0):
        print "writing macro no. %d of %d..." % (vox, np.prod(nvox))

    #read voxel information (no. & center coordinates)
    cc = np.array(vox_tab.readline().split()).astype(np.float)
    cc = cc[1:4]

    #create new macro file
    shutil.copy2('OptSim_raw.mac','macros/OptSim_%08d.mac' % vox)
    macro = open('macros/OptSim_%08d.mac' % vox, 'a')

    #write GEANT4 commands into macro file
    macro.write("/argoncube/gun/center %f %f %f mm\n" % (cc[0],cc[1],cc[2]))
    macro.write("/argoncube/gun/halfx " + str(vdim[0]/2.) + " mm\n")
    macro.write("/argoncube/gun/halfy " + str(vdim[1]/2.) + " mm\n")
    macro.write("/argoncube/gun/halfz " + str(vdim[2]/2.) + " mm\n")

    #number of photons per event
    macro.write("/argoncube/gun/primaryNb 10000\n")

    #number of events
    macro.write("/run/beamOn 100")

    macro.close()
