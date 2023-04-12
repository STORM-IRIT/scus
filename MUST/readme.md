# Scus (Spatially Coherent Ultrasound Simulation)
***
Extract scatterer from any position corresponding to an ultrasound probe
***

## Dependencies 
### Scatterer generation
```
(Ubuntu)
- libglm-dev
- libdcmtk-dev
- libtbb-dev

(Fedora)
- glm-devel
- dcmtk-devel
- tbb-devel
```

### Visualisation
```
pip install plotly
pip install pandas
```

***
***
## Compilation

```
mkdir build
cd build
cmake ..
make
```
***
***

## Usage
### Scatterer generation
```
./Scus <Setting File>.json
```
To generate the scatterers according to the settings file (see next section)

Note that the code may be used as an external library to generate scatterers or real-time ultrasound simulation.

### Visualisation
Use the `scatterer_visualisation.py` script to visualize scatterers generated using the `SPACE` or `PROJECTED` modes

### Simulation
Scatterers generated with the `RAW` are ready to be used with a standard ultrasound image simulator as in the examples provided

#### Field II
Write the scatterers file name in `scus_phantom.m` then use the functions `field_init(0)`, `mk_pht()` and `sim_img()`.
Additionally, python code to run multiple RF line simulation at once is also provided.

#### SIMUS
Simply use the `generateImageFromScats(filename)` function, the simulation parameters are adjustable in the .m file
***
***

## Settings file format
**Json file**


```
{
    "Grid" : { 
        see GRID
    },
    "Acquisition Zone" : {
        see ACQUISITION
    },
    "OutFile" : {
        "Mode": "SPACE"/"PROJECTED"/"RAW",  //SPACE/PROJECTED: For visualisation. RAW: To be used in simulations
        "Name": ...                         //name of the outfile
    }
}
```
***
**Grid**
 ```
 "Dimension":{                  //in mm
    "x":number,                 //multiples of CellDim
    "y":number,
    "z":number
 },
 
 "UseRot":true/false,           //random rotation of the cells
 "CellDim":number,              //mm
 "SamplesPerCell":number,
 "Sampler":"dart"/"regular"/"uniform"/"cascaded",
 "FixedSeed":number,            //-1 for time based random
 
 "WeightingObject":{
    see WEIGTHINGOBJECT
 }
 ```
***
**Acquition Zone**
```
"Dimension":{                   //mm
    "Width":number,
    "Height":number,
    "Depth":number
 },
 
"Position":{                    //mm
    "x":number,
    "y":number,
    "z":number
 },
 
 "Rotation":{
    "Angle":number              //degree
    
    "Axis":{                    //axis of the rotation, will be normalized
        "x":number,
        "y":number,
        "z":number
} 
```
***
**Weighting Object**
```
"Type":"Dicom"/"Plane"/"Sphere"/"Cube",
```
*Dicom* (Opening medical data in slices form)

```
"Files":[
    pathtodicom1,
    pathtodicom2,
    ...
    pathtodicomN
]
```
*Plane* (Medium seperated in two by a plane)
```
"Height":number                 //mm
```
*Sphere*
```
"Position":{                    //mm
    "x":number,
    "y":number,
    "z":number
 },
"Radius":number                 //mm
```
*Cube*
```
"Position":{                    //mm
    "x":number,
    "y":number,
    "z":number
 },
"Side":number                   //mm
```