import plotly.express as px
import sys

def lerp(a,b,t):
    return a + t(b*a)

def drawprobe(fig, probeX, probeY, probeZ,nbelem):
    for i in range(nbelem):
        drawmeshcube(fig,probeX[i*8:(i+1)*8],probeY[i*8:(i+1)*8],probeZ[i*8:(i+1)*8])

def drawmeshcube(fig,X,Y,Z):
    fig.add_mesh3d(x=X, y=Y, z=Z, i=[7, 0, 0, 0, 4, 4, 6, 6, 4, 0, 3, 2],
                   j=[3, 4, 1, 2, 5, 6, 5, 2, 0, 1, 6, 3],
                   k=[0, 7, 2, 3, 6, 7, 1, 1, 5, 5, 7, 6], color="red")


def drawcube(fig,offx,offz,offy) :
    cellx = list(map(lambda n: n +offx, [0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0]))
    celly = list(map(lambda n: n +offy, [0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0]))
    cellz = list(map(lambda n: n +offz, [0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1]))
    fig.add_scatter3d(x=cellx, y=celly, z=cellz, mode="lines",line=dict(color="mediumseagreen"))


def drawsquarex(fig,offx,offy,offz) :
    sqx = list(map(lambda n : n + offx, [0, 0, 0, 0, 0]))
    sqy = list(map(lambda n : n + offy, [0, 1, 1, 0, 0]))
    sqz = list(map(lambda n : n + offz, [0, 0, 1, 1, 0]))
    fig.add_scatter3d(x=sqx, y=sqy, z=sqz, mode="lines",line=dict(color="mediumseagreen"))


def drawsquarey(fig,offx,offy,offz) :
    sqx = list(map(lambda n : n + offx, [0, 1, 1, 0, 0]))
    sqy = list(map(lambda n : n + offy, [0, 0, 0, 0, 0]))
    sqz = list(map(lambda n : n + offz, [0, 0, 1, 1, 0]))
    fig.add_scatter3d(x=sqx, y=sqy, z=sqz, mode="lines",line=dict(color="mediumseagreen"))


def drawsquarez(fig,offx,offy,offz) :
    sqx = list(map(lambda n : n + offx, [0, 1, 1, 0, 0]))
    sqy = list(map(lambda n : n + offy, [0, 0, 1, 1, 0]))
    sqz = list(map(lambda n : n + offz, [0, 0, 0, 0, 0]))
    fig.add_scatter3d(x=sqx, y=sqy, z=sqz, mode="lines",line=dict(color="mediumseagreen"))



def plot_file(filename,save=False):
    x = []
    y = []
    z = []
    w = []
    c = []

    probeX = []
    probeY = []
    probeZ = []

    with open(filename, "r") as f:
        mode = f.readline()[0]
        line = f.readline()
        l = list(map(int, line.split()))
        gridDimX = l[0]
        gridDimY = l[1]
        gridDimZ = l[2]

        nbelem = int(f.readline())
        for i in range(nbelem*8):
            line = f.readline()
            l = list(map(float, line.split()))
            probeX.append(l[0])
            probeY.append(l[1])
            probeZ.append(l[2])

        while line := f.readline():
            l = list(map(float, line.split()))
            x.append(l[0])
            y.append(l[1])
            z.append(l[2])
            w.append(l[3])
            if len(l) > 4:
                c.append(l[4])

    cubex = list(map(lambda n: n * gridDimX, [0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0]))
    cubey = list(map(lambda n: n * gridDimY, [0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0]))
    cubez = list(map(lambda n: n * gridDimZ, [0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1]))

    px.defaults.color_continuous_scale = px.colors.sequential.gray

    if c:
        fig = px.scatter_3d(x=x, y=y, z=z, color=c)
    else:
        fig = px.scatter_3d(x=x, y=y, z=z, color=w)

    fig.update_traces(marker=dict(size=2))

    if mode == 's':
        fig.add_scatter3d(x=cubex, y=cubey, z=cubez, mode="lines",line=dict(color="red"))
        drawprobe(fig,probeX,probeY,probeZ,nbelem)




    fig.update_layout(scene_camera=dict(up=dict(x=0, y=1, z=0),

                                        eye=dict(x=0.4,y=1,z=2)))
    fig.update_scenes(aspectmode="data")

    fig.update(layout_coloraxis_showscale=False)
    fig.update(layout_showlegend=False)

    fig.update_layout(scene=dict(yaxis={'visible': False, 'showticklabels': False},xaxis={'visible': False, 'showticklabels': False},zaxis={'visible': False, 'showticklabels': False}))

    if save :
        fig.write_html("plot.html")
    else :
        fig.show()


if __name__ == "__main__":
    if len(sys.argv) > 1 :
        if len(sys.argv) > 2:
            plot_file(sys.argv[1],True)
        else :
            plot_file(sys.argv[1])
    else :
        plot_file(input("Enter scatterers filename :\n"))
        input("...")