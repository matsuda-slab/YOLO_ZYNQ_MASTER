PREF=xilinx_com_hls_yolo

all: gen_ip

gen_ip:
	bash scripts/create_all_ip.sh
	unzip ip/${PREF}_conv_dw_top_1_0.zip -d ip/${PREF}_conv_dw_top_1_0
	unzip ip/${PREF}_conv_pw_top_1_0.zip -d ip/${PREF}_conv_pw_top_1_0
	unzip ip/${PREF}_acc_top_1_0.zip -d ip/${PREF}_acc_top_1_0
	unzip ip/${PREF}_max_pool_top_1_0.zip -d ip/${PREF}_max_pool_top_1_0
	unzip ip/${PREF}_upsamp_top_1_0.zip -d ip/${PREF}_upsamp_top_1_0
	unzip ip/${PREF}_yolo_top_1_0.zip -d ip/${PREF}_yolo_top_1_0

clean:
	rm -f *.log *.jou
