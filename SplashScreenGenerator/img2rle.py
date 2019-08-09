from PIL import Image

def RLEencode(sequence):
    count = 1
    result = []
    for x,item in enumerate(sequence): 
        if x == 0:
            continue
        elif item == sequence[x - 1]:
            count += 1
        else:        
            result.append((sequence[x - 1], count))
            count = 1            
    result.append((sequence[len(sequence) - 1], count))
    return result

def MergeByte(sequence):
    result = []
    for item in sequence:
        colorbit=int(item[0])
        lenbit=item[1]

        tmp = (colorbit & 0xffff) << 14 # 两位颜色位,支持四色,占用开头 11000000 00000000
        tmp = tmp | lenbit # 连续长度
        result.append(tmp)
    return result

img = Image.open('BootScreen2.png')
rgb_im = img.convert('RGB')
pixels=[]
ColorTab=[]
for x in range(bmp_image.height):
   for y in range(bmp_image.width):
        r, g, b = rgb_im.getpixel((y, x))
        hasColorTab = False
        for idx in range(len(ColorTab)):
            ir,ig,ib = ColorTab[idx]
            if ir == r and ig == g and ib == b:
                pixels.append(str(idx))
                hasColorTab = True
                break
        if not hasColorTab:
            pixels.append(str(len(ColorTab)))
            ColorTab.append((r,g,b))

bmp_image.close()

ORLEenc = RLEencode(pixels)
ORLEenc = MergeByte(ORLEenc)


print("{")
count =0 
for x in ORLEenc:
    count +=1
    print("0x{:04x}, ".format(x),end = '')
    if count == 8:
        print()
        count=0
print("}")

print("Original size:%d Bytes"%len(pixels))
print("Original RLE encode size:%d(%d Bytes)"%(len(ORLEenc),len(ORLEenc)*2))
print("colortab:")
print(ColorTab)
