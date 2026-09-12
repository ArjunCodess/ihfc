"""Reproducible engineering schematics, not photographs or finished CAD."""
from pathlib import Path
from math import sin, cos, pi
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parent
OUT = ROOT / 'tmp/pdfs/full-design'
OUT.mkdir(parents=True, exist_ok=True)
INK = '#193f50'
MUTED = '#54636b'
BLUE = '#dcebf2'
TEAL = '#cde5df'
BG = '#ffffff'

def font(n, bold=False):
    return ImageFont.truetype('C:/Windows/Fonts/' + ('arialbd.ttf' if bold else 'arial.ttf'), n)

def label(d, xy, text, n=24, color=INK, bold=False):
    d.text(xy, text, font=font(n, bold), fill=color)

def arrow(d, a, b, color=INK, width=3):
    d.line([a, b], fill=color, width=width)
    dx, dy = b[0]-a[0], b[1]-a[1]
    length = (dx*dx+dy*dy)**.5
    ux, uy = dx/length, dy/length
    d.polygon([b,(b[0]-14*ux+6*uy,b[1]-14*uy-6*ux),
               (b[0]-14*ux-6*uy,b[1]-14*uy+6*ux)], fill=color)

im = Image.new('RGB', (1440, 820), BG); d = ImageDraw.Draw(im)
label(d,(55,30),'START: all equipment inside 480 x 280 mm',32,bold=True)
label(d,(55,77),'Plan view. Proposed envelopes include stowed mechanisms and wheels.',23,color=MUTED)
x0,y0,s=80,190,1.8
rect=lambda x,y,w,h:(x0+x*s,y0+y*s,x0+(x+w)*s,y0+(y+h)*s)
d.rectangle(rect(0,0,480,280),outline=INK,width=4)
for x,name,col in [(15,'SealBot',TEAL),(245,'CareBot',BLUE)]:
    d.rectangle(rect(x,5,220,200),fill=col,outline=INK,width=3)
    label(d,(x0+(x+30)*s,y0+70*s),name,29,bold=True)
    label(d,(x0+(x+24)*s,y0+102*s),'220 x 200 mm',23)
    arrow(d,(x0+(x+110)*s,y0+40*s),(x0+(x+110)*s,y0-32))
d.rectangle(rect(15,215,300,60),outline=MUTED,width=2)
d.rectangle(rect(25,215,280,20),fill='#8e7258',outline=INK,width=2)
d.rectangle(rect(25,255,250,20),fill='#b69875',outline=INK,width=2)
label(d,(990,230),'Both robots exit forward.',25,bold=True)
label(d,(990,281),'Beams remain behind',24)
label(d,(990,312),'until sample work ends.',24)
label(d,(990,379),'Beam strip: 300 x 60 mm',24)
label(d,(990,421),'20 mm between beams',24)
label(d,(990,489),'Kit batches are on CareBot.',24)
label(d,(80,724),'Occupied envelope: 450 x 270 mm. Use a real inspection frame before fabrication.',25,bold=True)
im.save(OUT/'start-layout.png')

im=Image.new('RGB',(1440,900),BG); d=ImageDraw.Draw(im)
label(d,(50,28),'CAREBOT: twelve numbered pockets, one transfer station',32,bold=True)
label(d,(50,78),'The stored colour belongs to the pocket record. Pocket number does not imply colour.',23,color=MUTED)
cx,cy,r=365,425,245
d.ellipse((cx-r,cy-r,cx+r,cy+r),fill=BLUE,outline=INK,width=4)
for i in range(12):
    a=pi/2+i*2*pi/12; px=cx+184*cos(a); py=cy+184*sin(a)
    d.ellipse((px-37,py-37,px+37,py+37),fill='white',outline=INK,width=3)
    label(d,(px-12,py-14),str(i),23,bold=True)
d.ellipse((cx-27,cy-27,cx+27,cy+27),fill=INK)
label(d,(cx-95,cy+50),'Supported axle',23)
d.rectangle((cx-43,cy+184-43,cx+43,cy+184+43),outline='#a34d2f',width=5)
arrow(d,(cx,cy+255),(cx,cy+324),'#a34d2f')
label(d,(174,775),'Front loading / release station',25,bold=True)
label(d,(710,182),'160 mm rotor; 120 mm pitch circle',27,bold=True)
label(d,(710,226),'Twelve 24-25 mm pockets, 30 degrees apart',23)
label(d,(710,266),'Fixed floor below; retaining lid above',23)
label(d,(710,306),'Real index marks prevent cumulative drift',23)
label(d,(710,375),'LOAD',26,bold=True)
label(d,(710,418),'Level cup lifts a confirmed patient.',24)
label(d,(710,451),'Cup floor opens over an empty pocket.',24)
label(d,(710,484),'Lower outlet gate remains closed.',24)
label(d,(710,556),'UNLOAD',26,bold=True)
label(d,(710,599),'Index a matching patient to the station.',24)
label(d,(710,632),'Open lower gate into the low exit chute.',24)
label(d,(710,665),'Verify empty, close gate, then index.',24)
label(d,(50,850),'Schematic only. Validate the lift path, pocket clearance and outlet alignment on the bench.',23,color=MUTED)
im.save(OUT/'patient-magazine.png')

im=Image.new('RGB',(1440,830),BG);d=ImageDraw.Draw(im)
label(d,(45,25),'RULEBOOK DIMENSION CHECK: closed beams versus tape',30,bold=True)
label(d,(45,75),'Nominal right-angle arrangement, measured from the bottom-left fixed-wall corner.',23,color=MUTED)
x0,y0,s=110,720,1.7
def box(x,y,w,h):
    return (x0+x*s,y0-(y+h)*s,x0+(x+w)*s,y0-y*s)
d.rectangle(box(0,0,300,300),fill='#f6f6f4')
d.rectangle(box(280,0,20,300),fill='#303030')
d.rectangle(box(0,280,300,20),fill='#303030')
d.line([(x0,y0-320*s),(x0,y0),(x0+330*s,y0)],fill=INK,width=7)
d.rectangle(box(250,0,20,280),fill='#b69875',outline=INK,width=2)
d.rectangle(box(0,260,250,20),fill='#8e7258',outline=INK,width=2)
label(d,(x0+65*s,y0-275*s),'250 mm',22,color='white',bold=True)
label(d,(725,211),'250 mm + 20 mm = 270 mm',29,bold=True)
label(d,(725,267),'The vertical beam stops 10 mm short',24)
label(d,(725,302),'of the drawn inner tape edge at 280 mm.',24)
label(d,(725,371),'The horizontal beam touches the other',24)
label(d,(725,406),'beam and the fixed left wall.',24)
label(d,(725,477),'The pair closes, but all tape-contact',24)
label(d,(725,512),'conditions are not satisfied.',24)
arrow(d,(785,587),(x0+275*s,y0-120*s),'#a34d2f')
label(d,(725,625),'Do not modify game pieces to fill the gap.',24,bold=True)
label(d,(45,785),'Source: beam lengths on page 10; 280 mm to inner tape edge on page 15. Not a validated scoring pose.',23,color=MUTED)
im.save(OUT/'beam-dimension-check.png')
print('Wrote three schematics to',OUT)
