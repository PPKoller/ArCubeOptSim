import subprocess
import numpy as np

#read voxel table
vox_tab = open('OptSim_LUT_voxel_table.txt', 'r')

cmin = np.array(vox_tab.readline().split()).astype(np.float)
cmax = np.array(vox_tab.readline().split()).astype(np.float)
vdim = np.array(vox_tab.readline().split()).astype(np.float)
nvox = np.array(vox_tab.readline().split()).astype(np.int)

print 'Number of voxels: %d' % np.prod(nvox)

gdml    = "./OptSim.gdml"
preinit = "./preinit.mac"

for vox in range(np.prod(nvox)):
#for vox in range(100):

    infile  = "./macros/OptSim_%08d.mac" % vox
    outfile = "./out/OptSim_%08d.root" % vox
    logfile = "./log/OptSim_%08d.log" % vox
    errfile = "./log/OptSim_%08d.txt" % vox

    with open(logfile,"wb") as out, open(errfile, "wb") as err:
        subprocess.Popen(["../../build/ArgonCubeOptPh",
            "-g", gdml,
            "-p", preinit,
            "-m", infile,
            "-o", outfile,
            ], stdout=out, stderr=err)

