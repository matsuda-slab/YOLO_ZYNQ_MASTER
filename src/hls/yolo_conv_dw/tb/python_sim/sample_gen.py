import os
import torch
import torch.nn as nn
import numpy as np

WIDTH    = 16
HEIGHT   = 16
CHANNELS = 16

HEADER_FILE = "tb_sample.h"

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

    # initialize weight by sample weight
    conv_dw.state_dict()['weight'][0:CHANNELS] = weight

    # Conv Calculation
    output = conv_dw(input)
    # Convert to Fixed precision
    output = (output * (2 ** 8)).to(torch.short).to(torch.float32) / (2 ** 8)

    # Convert torch tensor to ndarray
    input = input.detach().numpy().copy()
    weight = weight.detach().numpy().copy()
    output = output.detach().numpy().copy()

    np.set_printoptions(threshold=np.inf)
    print("output: ", output)

    input_interleaved = interleave(input)
    output_interleaved = interleave(output)
    print("output_interleaved: ", output_interleaved)

    if os.path.exists(HEADER_FILE):
        os.remove(HEADER_FILE)

    make_header(HEADER_FILE, input_interleaved, "tb_input")
    make_header(HEADER_FILE, weight, "tb_weight")
    make_header(HEADER_FILE, output_interleaved, "tb_output")

def interleave(input):
    input_interleaved = np.zeros(input.shape[2] * input.shape[3] * input.shape[1])
    pix_cnt = 0
    for h in range(input.shape[2]):
        for w in range(input.shape[3]):
            for c in range(input.shape[1]):
                input_interleaved[pix_cnt] = input[0][c][h][w]
                pix_cnt += 1

    return input_interleaved

def make_header(filename, input, val_name):
    with open(filename, "a") as f:
        f.write(f"float {val_name}[{input.size}] = {{\n")
        input = np.ravel(input)

        for oc in range(input.shape[0]):
            if oc == input.shape[0] - 1:
                f.write(f"{input[oc]}}};\n")
            else:
                f.write(f"{input[oc]},\n")

if __name__ == "__main__":
    main()
