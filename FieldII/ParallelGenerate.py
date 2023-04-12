import matlab.engine

print("Launching Matlab...")
eng = matlab.engine.start_matlab()

print("Matlab launched")
eng.field_init(0)

eng.sim_img(nargout=0)
