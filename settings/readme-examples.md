This folder contains examples on how to use the program to generate scatterers.

Three use-case are presented :

    - Visualisation of the scatterers in the probe coordinate system using DICOM
    - Visualisation of the scatterers in the global coordinate system using a cube and a tilted probe
    - Generation of scatterers to be used in off-line simulation using a sphere

---

**General Usage**

Once the program is compiled, each .json file can be used to produce the described result using:

`.\Scus <path/to/file>.json`

All those files can be modified according to the main readme instructions.

---

**DICOM visualisation**

*File:* 
settings_preview_projectedDICOM.json 

***/!\\*** - This configuration file needs to be filled with actual DICOM slice files.

Additionally, only DICOM in the form of one slice by file are accepted, they will be read as stacked bottom up in the given order.

*Out:* a *preview_projectedDICOM.dat* to be used with the scatterer_visualisation.py script.

 This file contains scatterers generated and extracted to simulate the DICOM region described, projected in the probe system coordinate.

---

**Tilted Cube in probe coordinate**

*File:*
settings_preview_tiltedCube.json

*Out:*
a *tiltedCubeScats_preview.dat* to be used with the scatterer_visualisation.py script.

This file contains scatterers generated for a tiled probe crossing a cube. the visualisation is done in global coordinate system, with the grid displayed.

---

**Simulation with a Sphere**

*File:* settings_simu_sphere.json

*Out:*
a sphereScats.dat to be used with FieldII/Simus to simulate an ultrasound image using the generated scatterers. This file represents an imaging plane crossing a sphere in its center.