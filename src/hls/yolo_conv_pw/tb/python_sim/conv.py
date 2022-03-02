import torch
import torch.nn as nn
import numpy as np

in_ch = 4
out_ch = 8
in_img_out_ch  = in_ch
in_img_in_ch = 1
in_img_h = 16
in_img_w = 16

###https://qiita.com/mathlive/items/d9f31f8538e20a102e14###

def main():
    #file open
    file = open("./head/conv_tb.h","a")

    #conv main
    conv_tb = nn.Conv2d(   in_ch,   out_ch, kernel_size=1, stride=1, padding=0, bias=0)#conv layer instance 
    input = torch.randn(in_img_in_ch,in_img_out_ch,in_img_w,in_img_h)#入力値を乱数で作成
    input = (((input * (2**8)).to(torch.int)).to(torch.float32))/(2**8)#入力値を量子化
    conv_tb.state_dict()["weight"][0:out_ch] = (((torch.randn(out_ch,in_ch,1,1) * (2**8)).to(torch.int)).to(torch.float32))/(2**8)#重みを乱数で初期化->量子化
    output = conv_tb(input) #畳み込み処理

    stdict = conv_tb.state_dict() #weight 抽出
    weights    = stdict["weight"].numpy().copy()

    #wirte to file
    print("input  shape:",input.shape)
    print("output shape:",output.shape)
    #print(conv_tb)
    #print(conv_tb.state_dict())
    #print(torch.max(input),torch.max(output),torch.max(stdict["weight"]))

    #Cヘッダファイル作成
    input = input.to('cpu').detach().numpy().copy()
    output = output.to('cpu').detach().numpy().copy()

    input_interleave = interleave(input)
    output_interleave = interleave(output)
    #print(input[0][1][0][0])
    #print(input_interleave[1])
    make_headflie(file,input_interleave,"tb_input")
    make_headflie(file,output_interleave,"tb_output")
    make_headflie(file,weights,"tb_weights")


def interleave(input):
    ###out_ch_cnt = 0
    ###for i in range(input.shape[1]):
    ###    for j in range(input.shape[2]):
    ###        for k in range(input.shape[3]):
    ###            input_interleave[0][out_ch_cnt] = input[0][i][j][k]
    ###            print(out_ch_cnt)
    ###            if(input.shape[1]-1 == out_ch_cnt):
    ###                out_ch_cnt = 0
    ###            else:
    ###                out_ch_cnt += 1
    input_interleave_flat = np.zeros(input.shape[1]*input.shape[2]*input.shape[3])
    pixel_cnt = 0
    for k in range(input.shape[3]):                                
        for j in range(input.shape[2]):                            
            for i in range(input.shape[1]):                        
                input_interleave_flat[pixel_cnt] = input[0][i][k][j]
                #print(i,k,j)
                pixel_cnt += 1
    
    return input_interleave_flat

def make_headflie(file, input, name):
    if input.ndim ==4:
        file.write("float "+name+" ["+str(input.shape[0]*input.shape[1]*input.shape[2]*input.shape[3])+"] ={")
    elif input.ndim ==1:
        file.write("float "+name+" ["+str(input.shape[0])+"] ={")
    input = np.ravel(input)
       
    for out_ch  in range(input.shape[0]):
        if not out_ch+1 == input.shape[0]:
            file.write(str(input[out_ch])+",\n")
        else:
            file.write(str(input[out_ch])+"};\n")

if __name__ == "__main__":
    main()
