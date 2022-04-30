import numpy as np
import vispy.scene
from vispy import app
from vispy.scene import visuals

def openCsv():
    f = open("output.csv", "r")
    f.readline()
    return f

def readCsvLine(f):
    line = f.readline()
    if(len(line) == 0):
        return None
    nums = list(map(float, line.split(",")))
    nums = nums[1:]
    points = list(nums[i:i+3] for i in range(0, len(nums), 3))
    return np.array(points)

csvFile = openCsv()

pos = readCsvLine(csvFile)

canvas = vispy.scene.SceneCanvas(keys='interactive', show=True)
view = canvas.central_widget.add_view()

scatter = visuals.Markers()
scatter.set_data(pos, edge_color=None, face_color=(1, 1, 1, .5), size=5)

view.add(scatter)

view.camera = 'turntable'  # or try 'arcball'

# add a colored 3D axis for orientation
axis = visuals.XYZAxis(parent=view.scene)

def update(ev):

    global csvFile

    pos = readCsvLine(csvFile)
    if(pos is None):
        csvFile.close()
        csvFile = openCsv()
        pos = readCsvLine(csvFile)

    scatter.set_data(pos, edge_color=None, face_color=(1, 1, 1, .5), size=5)
    idx += 1

timer = app.Timer()
timer.connect(update)
timer.start(0)

if __name__ == '__main__':
    import sys
    if sys.flags.interactive != 1:
        vispy.app.run()

