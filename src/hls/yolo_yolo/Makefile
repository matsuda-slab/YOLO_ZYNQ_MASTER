all:
	vivado_hls -f tcl/run_hls.tcl
	
sim:
	vivado_hls -f tcl/sim.tcl

syn:
	vivado_hls -f tcl/synth.tcl

cosim:
	vivado_hls -f tcl/cosim.tcl

clean:
	rm -f *.log
