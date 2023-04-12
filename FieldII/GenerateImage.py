import matlab.engine
import shutil
import os
import subprocess
import time

import visualisationV2

NB_INSTANCES = 5

# Copy points to folder
file = "../cmake-build-release/raw_res.dat"

if os.path.exists(file) :
    shutil.copy(file, "raw_res.dat")
else:
    print("Unable to locate :",file, "use point generation tool to create it")
    exit(-1)

# Clear previous rf (with backup)
folder = "rf_data"
if not os.path.exists(folder):
    os.mkdir(folder)
else :
    if os.path.exists(folder + "_old"):
        shutil.rmtree(folder + "_old")
    os.rename(folder, folder + "_old")
    os.mkdir(folder)

for filename in os.listdir(folder):
    file_path = os.path.join(folder, filename)
    try:
        if os.path.isfile(file_path) or os.path.islink(file_path):
            os.unlink(file_path)
        elif os.path.isdir(file_path):
            shutil.rmtree(file_path)
    except Exception as e:
        print("Failed to delete %s. Reason: %s" % (file_path, e))

# Create first matlab instance
# Translate .dat to .mat
eng = matlab.engine.start_matlab()
eng.mk_pht(nargout=0)
print("Phantom updated")

# Creating more instances
processes = []
for i in range(NB_INSTANCES):
    processes.append(subprocess.Popen(["Python", "ParallelGenerate.py"]))

print("Process launched")
print("Waiting for processes...")

#visualisationV2.plot_file("../cmake-build-release/res.dat")

for proc in processes:
    proc.wait()
    print("A process finished")

eng.make_image(nargout=0)


input("End simulation")
eng.quit()
