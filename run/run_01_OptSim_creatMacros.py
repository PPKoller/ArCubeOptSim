import shutil
import numpy as np

#read voxel table
vox_tab = open('OptSim_LUT_voxel_table.txt', 'r')

cmin = np.array(vox_tab.readline().split()).astype(np.float)
cmax = np.array(vox_tab.readline().split()).astype(np.float)
vdim = np.array(vox_tab.readline().split()).astype(np.float)
nvox = np.array(vox_tab.readline().split()).astype(np.int)
n = np.array(vox_tab.readline().split()).astype(np.int)

print 'Number of voxels: %d' % np.prod(nvox)
print 'Number of events: %d' % n[0]
print 'Number of photons/evt: %d' % n[1]

for vox in range(np.prod(nvox)):

    #read voxel information (no. & center coordinates)
    line = np.array(vox_tab.readline().split()).astype(np.float)
    nv = line[0] # voxel number
    cv = line[1:4] # voxel coordinates origin

    #only write macros asked for
    if nv not in range(0,10):
        continue;

    if (vox%100 == 0):
        print "writing macro no. %d of %d..." % (vox, np.prod(nvox))

    #create new macro file
    shutil.copy2('macros/OptSim_template.mac','macros/OptSim_%08d.mac' % nv)
    macro = open('macros/OptSim_%08d.mac' % nv, 'a')

    #write GEANT4 commands into macro file
    macro.write("/argoncube/gun/center %f %f %f mm\n" % (cv[0],cv[1],cv[2]))
    macro.write("/argoncube/gun/halfx " + str(vdim[0]/2.) + " mm\n")
    macro.write("/argoncube/gun/halfy " + str(vdim[1]/2.) + " mm\n")
    macro.write("/argoncube/gun/halfz " + str(vdim[2]/2.) + " mm\n")

    #number of photons per event
    macro.write("/argoncube/gun/primaryNb %d\n" % n[1])

    #number of events
    macro.write("/run/beamOn %d" % n[0])

    macro.close()
