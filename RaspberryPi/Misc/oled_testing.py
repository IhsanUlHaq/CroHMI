import time
import Adafruit_SSD1306
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

RST = 0

disp = Adafruit_SSD1306.SSD1306_128_64(rst=RST)
disp.begin()
disp.clear()
disp.display()

width = disp.width
height = disp.height

image1 = Image.new('1', (width, height))

draw = ImageDraw.Draw(image1)
draw.rectangle((0, 0, width, height), outline=0, fill=0)

padding = -2
top = padding

bottom = height-padding
x=0
font = ImageFont.truetype('Helvetica.ttf', 12)

disp.clear()
disp.display()
draw.text((x, top+4), "Rcvd frm: Node 1", font = font, fill = 255)
draw.text((x, top+16), "Air: 42.0C, 69%", font = font, fill = 255)
draw.text((x, top+28), "Soil: 42.0C, 69%", font = font, fill = 255)
draw.text((x, top+40), "Battery: 11.8V", font = font, fill = 255)
draw.text((x, top+52), "LDB: OK, RDB: Err", font = font, fill = 255)

draw.rectangle((124,0,128,12), outline=0, fill=255)
draw.rectangle((120,4,124,12), outline=0, fill=255)
draw.rectangle((116,8,120,12), outline=0, fill=255)
draw.rectangle((116,8,128,10), outline=0, fill=255)

disp.image(image1)
disp.display()
time.sleep(2)

