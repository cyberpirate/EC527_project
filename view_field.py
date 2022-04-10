import numpy as np
import vispy.scene
from vispy import app
from vispy.scene import visuals

lines = []

with open("output.csv", "r") as f:
    lines = f.readlines()

if len(lines) == 0:
    exit()

lines = lines[1:]

lines = filter(lambda x: len(x) > 1000, lines)
lines = map(lambda x: x.split(","), lines)
lines = map(lambda x: x[1:], lines)
lines = map(lambda x: list(map(float, x)), lines)
lines = map(lambda x: list(x[i:i+3] for i in range(0, len(x), 3)), lines)
lines = map(lambda x: np.array(x), lines)

lines = list(lines)

# lineNums = list(map(len, lines))
# lineNums = list(enumerate(lineNums))
# lineNums = list(filter(lambda x: x[1] != 1000, lineNums))

# for i in range(len(lines)-1):
#     print(np.sum(np.abs(lines[i]) - np.abs(lines[i+1])))

# exit()

pos = lines[0]

canvas = vispy.scene.SceneCanvas(keys='interactive', show=True)
view = canvas.central_widget.add_view()

scatter = visuals.Markers()
scatter.set_data(pos, edge_color=None, face_color=(1, 1, 1, .5), size=5)

view.add(scatter)

view.camera = 'turntable'  # or try 'arcball'

# add a colored 3D axis for orientation
axis = visuals.XYZAxis(parent=view.scene)

idx = 0

def update(ev):
    global idx
    pos = lines[idx % len(lines)]
    scatter.set_data(pos, edge_color=None, face_color=(1, 1, 1, .5), size=5)
    idx += 1

timer = app.Timer()
timer.connect(update)
timer.start(0)

if __name__ == '__main__':
    import sys
    if sys.flags.interactive != 1:
        vispy.app.run()

