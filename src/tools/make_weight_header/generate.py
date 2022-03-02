"""
ndarrayファイル (*.npy) から Zynq_YOLO 用のヘッダファイルを作成する
Separableバージョンのみ対応．
"""

import os, sys
import numpy as np

def main():
    npy_file = sys.argv[1]

    params = np.load(npy_file, allow_pickle=True)
    params = params.item()

    name_tbl = {'conv1.conv_dw.weight':'group_0_0_weights',
                'conv1.conv_dw.bias':'group_0_0_biases',
                'conv1.conv_pw.weight':'group_0_1_weights',
                'conv1.conv_pw.bias':'group_0_1_biases',
                'conv2.conv_dw.weight':'group_1_0_weights',
                'conv2.conv_dw.bias':'group_1_0_biases',
                'conv2.conv_pw.weight':'group_1_1_weights',
                'conv2.conv_pw.bias':'group_1_1_biases',
                'conv3.conv_dw.weight':'group_2_0_weights',
                'conv3.conv_dw.bias':'group_2_0_biases',
                'conv3.conv_pw.weight':'group_2_1_weights',
                'conv3.conv_pw.bias':'group_2_1_biases',
                'conv4.conv_dw.weight':'group_3_0_weights',
                'conv4.conv_dw.bias':'group_3_0_biases',
                'conv4.conv_pw.weight':'group_3_1_weights',
                'conv4.conv_pw.bias':'group_3_1_biases',
                'conv5.conv_dw.weight':'group_4_0_weights',
                'conv5.conv_dw.bias':'group_4_0_biases',
                'conv5.conv_pw.weight':'group_4_1_weights',
                'conv5.conv_pw.bias':'group_4_1_biases',
                'conv6.conv_dw.weight':'group_6_0_weights',
                'conv6.conv_dw.bias':'group_6_0_biases',
                'conv6.conv_pw.weight':'group_6_1_weights',
                'conv6.conv_pw.bias':'group_6_1_biases',
                'conv7.conv_dw.weight':'group_7_0_weights',
                'conv7.conv_dw.bias':'group_7_0_biases',
                'conv7.conv_pw.weight':'group_7_1_weights',
                'conv7.conv_pw.bias':'group_7_1_biases',
                'conv8.conv.weight':'group_8_weights',
                'conv8.conv.bias':'group_8_biases',
                'conv9.conv_dw.weight':'group_9_0_weights',
                'conv9.conv_dw.bias':'group_9_0_biases',
                'conv9.conv_pw.weight':'group_9_1_weights',
                'conv9.conv_pw.bias':'group_9_1_biases',
                'conv10.weight':'group_10_weights',
                'conv10.bias':'group_10_biases',
                'conv11.conv.weight':'group_11_weights',
                'conv11.conv.bias':'group_11_biases',
                'conv12.conv_dw.weight':'group_12_0_weights',
                'conv12.conv_dw.bias':'group_12_0_biases',
                'conv12.conv_pw.weight':'group_12_1_weights',
                'conv12.conv_pw.bias':'group_12_1_biases',
                'conv13.weight':'group_13_weights',
                'conv13.bias':'group_13_biases'
                }

    for key in params.keys():
        print(key, f"({params[key].shape})")
        param = params[key]
        dim = param.ndim

        # Weight
        if dim == 4:
            OC, IC, FH, FW = param.shape
            ic_fold_factor = 1 if IC < 32 else int(IC / 32)
            MAX_CH = IC if IC < 32 else 32

            if FH == 3:             # 3x3
                size = OC * IC * 3 * 4
            else:                   # 1x1
                if OC == 16:         # 1st layer
                    size = OC * (IC+1)      # 64
                else:
                    size = OC * IC

            file_name = name_tbl[key]
            file_path = os.path.join('headers', file_name + ".h")

            cnt = 0
            with open(file_path, "w") as f:
                # Depthwise
                if "dw" in key:
                    f.write(f"short {file_name}[{size}] = {{")
                    for oc in range(OC):
                        for fh in range(FH):
                            for fw in range(FW):
                                val = param[oc][0][fh][fw]
                                if cnt == OC*IC*(FH+1)*FW - 1:
                                    f.write(f"{val}}}")
                                else:
                                    f.write(f"{val},\n")
                                cnt += 1
                            if fh == 2:
                                for fw in range(FW):
                                    if cnt == OC*IC*(FH+1)*FW - 1:
                                        f.write("0};\n")
                                    else:
                                        f.write("0,\n")
                                    cnt += 1

                # Pointwise
                else:
                    if IC == 3:
                        CNT_MAX = OC*4
                    else:
                        CNT_MAX = OC*IC

                    # YOLO Layer
                    if OC == 18:
                        size = IC * 256
                        f.write(f"short {file_name}[{size}] = {{")
                        for factor in range(ic_fold_factor):
                            for ic in range(MAX_CH):
                                val = param[0][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[1][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[2][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[3][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[4][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[5][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                for oc in range(79):
                                    f.write("0,\n")
                            for ic in range(MAX_CH):
                                val = param[6][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[7][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[8][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[9][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[10][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[11][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                for oc in range(79):
                                    f.write("0,\n")
                            for ic in range(MAX_CH):
                                val = param[12][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[13][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[14][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[15][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[16][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                val = param[17][factor*MAX_CH+ic][0][0]
                                f.write(f"{val},\n")
                            for ic in range(MAX_CH):
                                for oc in range(79):
                                    f.write("0,\n")
                            for ic in range(MAX_CH):
                                if (ic == MAX_CH - 1) and (factor == ic_fold_factor - 1):
                                    f.write("0};")
                                else:
                                    f.write("0,\n")

                    else:
                        f.write(f"short {file_name}[{size}] = {{")
                        for factor in range(ic_fold_factor):
                            for oc in range(OC):
                                for ic in range(MAX_CH):
                                    val = param[oc][factor*MAX_CH+ic][0][0]
                                    if cnt == CNT_MAX - 1:
                                        f.write(f"{val}}};")
                                    else:
                                        f.write(f"{val},\n")
                                    cnt += 1
                                if IC == 3:         # layer 1
                                    if cnt == CNT_MAX - 1:
                                        f.write("0};")
                                    else:
                                        f.write("0,\n")
                                    cnt += 1

        # Bias
        else:
            file_name = name_tbl[key]
            file_path = os.path.join('headers', file_name + ".h")

            OC = 4 if param.shape[0] == 3 else param.shape[0]
            cnt = 0
            with open(file_path, "w") as f:
                if param.shape[0] == 3:
                    CNT_MAX = 4
                else:
                    CNT_MAX = param.shape[0]

                # YOLO Layer
                if param.shape[0] == 18:
                    f.write(f"short {file_name}[256] = {{")
                    f.write(f"{param[0]},\n")
                    f.write(f"{param[1]},\n")
                    f.write(f"{param[2]},\n")
                    f.write(f"{param[3]},\n")
                    f.write(f"{param[4]},\n")
                    f.write(f"{param[5]},\n")
                    for oc in range(79):
                        f.write("0,\n")
                    f.write(f"{param[6]},\n")
                    f.write(f"{param[7]},\n")
                    f.write(f"{param[8]},\n")
                    f.write(f"{param[9]},\n")
                    f.write(f"{param[10]},\n")
                    f.write(f"{param[11]},\n")
                    for oc in range(79):
                        f.write("0,\n")
                    f.write(f"{param[12]},\n")
                    f.write(f"{param[13]},\n")
                    f.write(f"{param[14]},\n")
                    f.write(f"{param[15]},\n")
                    f.write(f"{param[16]},\n")
                    f.write(f"{param[17]},\n")
                    for oc in range(79):
                        f.write("0,\n")
                    f.write("0};")

                else:
                    f.write(f"short {file_name}[{OC}] = {{")
                    for oc in range(param.shape[0]):
                        if cnt == CNT_MAX - 1:
                            f.write(f"{param[oc]}}};")
                        else:
                            f.write(f"{param[oc]},\n")
                        cnt += 1
                    if param.shape[0] == 3:
                        f.write("0};")
                        cnt += 1

if __name__ == "__main__":
    main()
