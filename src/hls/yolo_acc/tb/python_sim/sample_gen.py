import os
import torch
import torch.nn as nn
import numpy as np

WIDTH    = 16
HEIGHT   = 16
CHANNELS = 3

HEADER_FILE = "tb_sample.h"

def accumulation(x, bias):
    acc_output = torch.zeros((1, x.shape[1], x.shape[2], x.shape[3]))
    for c in range(x.shape[1]):
        acc_output[0][c] = x[0][c] + bias[c]
    acc_output[acc_output < 0] = acc_output[acc_output < 0] * 0.1

    return acc_output

def main():
    # conv_dw instance
    conv_dw = nn.Conv2d(CHANNELS, CHANNELS, kernel_size=3, groups=CHANNELS,
                        stride=1, padding=1, bias=0)

    # Generate test input
    input = torch.randn(1, CHANNELS, HEIGHT, WIDTH)
    # Convert to Fixed precision
    input = (input * (2 ** 8)).to(torch.short).to(torch.float32) / (2 ** 8)

    # Generate test weight
    weight = torch.randn(CHANNELS, 1, 3, 3)
    # Convert to Fixed precision
    weight = (weight * (2 ** 8)).to(torch.short).to(torch.float32) / (2 ** 8)

    # Generate test bias
    bias = torch.randn(CHANNELS)
    # Convert to Fixed precision
    bias = (bias * (2 ** 8)).to(torch.short).to(torch.float32) / (2 ** 8)

    # initialize weight by sample weight
    conv_dw.state_dict()['weight'][0:CHANNELS] = weight

    # Conv Calculation
    acc_input_a = conv_dw(input)
    # Convert to Fixed precision
    acc_input_a = (acc_input_a * (2 ** 8)).to(torch.short).to(torch.float32) / (2 ** 8)

    acc_input_b = np.zeros(WIDTH*HEIGHT*4, dtype=np.int16)

    output = accumulation(acc_input_a, bias)

    # Convert to short
    acc_input_a = (acc_input_a * (2 ** 8)).to(torch.short)
    output = (output * (2 ** 8)).to(torch.short)
    bias = (bias * (2 ** 8)).to(torch.short)

    # Convert torch tensor to ndarray
    acc_input_a  = acc_input_a.detach().numpy().copy()
    weight = weight.detach().numpy().copy()
    output = output.detach().numpy().copy()
    bias = bias.detach().numpy().copy()

    acc_input_a_interleaved = interleave(acc_input_a)
    output_interleaved = interleave(output)
    weight_padded = weight_padding(weight)
    bias_padded = bias_padding(bias)

    if os.path.exists(HEADER_FILE):
        os.remove(HEADER_FILE)

    make_header(HEADER_FILE, acc_input_a_interleaved, "tb_input_a")
    make_header(HEADER_FILE, acc_input_b, "tb_input_b")
    #make_header(HEADER_FILE, weight_padded, "tb_weight")
    make_header(HEADER_FILE, bias_padded, "tb_bias")
    make_header(HEADER_FILE, output_interleaved, "tb_output")

def weight_padding(weight):
    weight_padded = np.zeros(weight.shape[0] * 3 * 4, dtype=np.int16)
    pix_cnt = 0
    for c in range(weight.shape[0]):
        for kh in range(weight.shape[2]):
            for kw in range(weight.shape[3]):
                weight_padded[pix_cnt] = weight[c][0][kh][kw];        
                pix_cnt += 1
        for z in range(3):
            weight_padded[pix_cnt] = 0
            pix_cnt += 1

    return weight_padded

def bias_padding(bias):
    if bias.shape[0] == 3:
        bias_padded = np.zeros(4, dtype=np.int16)
        for i in range(3):
            bias_padded[i] = bias[i]
        bias_padded[3] = 0
        return bias_padded
    else:
        return bias

def interleave(input):
    if input.shape[1] == 3:
        size = input.shape[2] * input.shape[3] * 4
    else:
        size = input.shape[2] * input.shape[3] * input.shape[1]
    input_interleaved = np.zeros(size, dtype=np.int16)
    pix_cnt = 0
    for h in range(input.shape[2]):
        for w in range(input.shape[3]):
            for c in range(input.shape[1]):
                input_interleaved[pix_cnt] = input[0][c][h][w]
                pix_cnt += 1
            if input.shape[1] == 3:
                input_interleaved[pix_cnt] = 0
                pix_cnt += 1

    return input_interleaved

def make_header(filename, input, val_name):
    with open(filename, "a") as f:
        f.write(f"short {val_name}[{input.size}] = {{\n")
        input = np.ravel(input)

        for oc in range(input.shape[0]):
            if oc == input.shape[0] - 1:
                f.write(f"{input[oc]}}};\n")
            else:
                f.write(f"{input[oc]},\n")

if __name__ == "__main__":
    main()
