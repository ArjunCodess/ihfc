"""Build the printable ground-up robot proposal from its Markdown source."""
from pathlib import Path
import re
from html import escape
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, KeepTogether, PageBreak
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.enums import TA_LEFT
from reportlab.graphics.shapes import Drawing, Rect, Line, String, Circle

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / 'TWO-ROBOT-MINIMAL-BUILD.md'
OUTPUT = ROOT / 'output/pdf/TWO-ROBOT-MINIMAL-BUILD.pdf'
OUTPUT.parent.mkdir(parents=True, exist_ok=True)
INK = colors.HexColor('#193f50')
MUTED = colors.HexColor('#52636c')
LIGHT = colors.HexColor('#edf2f4')
TEAL = colors.HexColor('#bddfd6')
BLUE = colors.HexColor('#cadfed')
styles = getSampleStyleSheet()
styles.add(ParagraphStyle(name='BodyRobot', fontName='Helvetica', fontSize=9.5, leading=13.5, spaceAfter=7))
styles.add(ParagraphStyle(name='SectionRobot', fontName='Helvetica-Bold', fontSize=15, leading=19, textColor=INK, spaceBefore=15, spaceAfter=9, keepWithNext=True))
styles.add(ParagraphStyle(name='SubRobot', fontName='Helvetica-Bold', fontSize=11, leading=15, textColor=INK, spaceBefore=10, spaceAfter=6, keepWithNext=True))
styles.add(ParagraphStyle(name='CellRobot', fontName='Helvetica', fontSize=8, leading=11, spaceAfter=0))
styles.add(ParagraphStyle(name='CaptionRobot', fontName='Helvetica', fontSize=8, leading=11, textColor=MUTED, spaceAfter=9))
styles.add(ParagraphStyle(name='ListRobot', parent=styles['BodyRobot'], leftIndent=15, firstLineIndent=-10))
styles.add(ParagraphStyle(name='TitleRobot', fontName='Helvetica-Bold', fontSize=28, leading=33, textColor=INK, spaceAfter=18))
for style in ['BodyRobot','ListRobot','CellRobot','CaptionRobot']:
    styles[style].allowWidows=0
    styles[style].allowOrphans=0

def inline(s):
    out, at = [], 0
    for m in re.finditer(r'\[([^]]+)\]\((https?://[^)]+)\)', s):
        out.append(escape(s[at:m.start()]))
        out.append(f'<link href="{escape(m[2], quote=True)}" color="#245e80">{escape(m[1])}</link>')
        at=m.end()
    out.append(escape(s[at:]))
    rendered = ''.join(out)
    return re.sub(r'\*\*([^*]+)\*\*', r'<b>\1</b>', rendered)

def p(s, style='BodyRobot'):
    return Paragraph(inline(s), styles[style])

def label(d, x, y, text, size=9, color=INK, anchor='start'):
    d.add(String(x,y,text,fontName='Helvetica',fontSize=size,fillColor=color,textAnchor=anchor))

def start_diagram():
    d=Drawing(475,325)
    s=.88; ox=20; oy=38
    def box(x,y,w,h,fill):
        d.add(Rect(ox+x*s,oy+y*s,w*s,h*s,fillColor=fill,strokeColor=INK,strokeWidth=.8))
    box(0,0,480,280,colors.white)
    box(10,10,200,260,TEAL)
    box(220,20,230,240,BLUE)
    box(45,10,20,260,colors.HexColor('#cdbb9c'))
    box(80,25,20,230,colors.HexColor('#b7a584'))
    label(d,ox+110*s,oy+150*s,'SealBot',14,anchor='middle')
    label(d,ox+335*s,oy+158*s,'CareBot',14,anchor='middle')
    label(d,ox+110*s,oy+130*s,'beams inside envelope',9,anchor='middle')
    label(d,ox+335*s,oy+138*s,'lift + 7 tilted boxes',9,anchor='middle')
    for u in [110,335]:
        d.add(Line(ox+u*s,oy+200*s,ox+u*s,oy+235*s,strokeColor=INK,strokeWidth=1.5))
        d.add(Line(ox+u*s,oy+235*s,ox+(u-5)*s,oy+228*s,strokeColor=INK))
        d.add(Line(ox+u*s,oy+235*s,ox+(u+5)*s,oy+228*s,strokeColor=INK))
    label(d,ox+55*s,oy+18*s,'beam A',8,anchor='middle')
    label(d,ox+90*s,oy+33*s,'beam B',8,anchor='middle')
    label(d,ox+240*s,oy+280*s+12,'480 mm',10,anchor='middle')
    label(d,ox+480*s+8,oy+140*s,'280',9)
    label(d,20,12,'Top view. Pair envelope is 440 x 260 mm including a 10 mm gap. Gauge the loaded beams physically.',8)
    return d

def care_diagram():
    d=Drawing(475,215)
    boxes=[(8,'Shared mouth',TEAL),(99,'Belt lift',TEAL),(190,'Sensor cell',BLUE),(281,'Selector',BLUE),(372,'7 boxes',TEAL)]
    for x,t,c in boxes:
        d.add(Rect(x,135,82,42,fillColor=c,strokeColor=INK))
        label(d,x+41,152,t,9,anchor='middle')
        if x<372:
            d.add(Line(x+83,156,x+90,156,strokeColor=INK))
            d.add(Line(x+90,156,x+85,160,strokeColor=INK))
            d.add(Line(x+90,156,x+85,152,strokeColor=INK))
    label(d,8,111,'Kits and patients use one lift, one reading cell, and one selector.',9)
    d.add(Rect(8,45,205,38,fillColor=BLUE,strokeColor=INK))
    label(d,110,60,'3 kit boxes: H 6 | PCC-A 2 | PCC-B 2',8,anchor='middle')
    d.add(Rect(230,45,225,38,fillColor=TEAL,strokeColor=INK))
    label(d,342,60,'4 patient boxes: red | green | yellow-A | yellow-B',8,anchor='middle')
    label(d,8,18,'Every box is tilted and has its own simple servo outlet gate for batch dumping.',8)
    return d

def sample_diagram():
    d=Drawing(475,225)
    label(d,18,202,'SealBot side layout, front at left',12)
    d.add(Rect(18,112,90,58,fillColor=TEAL,strokeColor=INK))
    label(d,63,145,'input flap',10,anchor='middle')
    label(d,63,128,'moves up/down',8,anchor='middle')
    d.add(Rect(18,55,42,35,fillColor=colors.HexColor('#cdbb9c'),strokeColor=INK))
    d.add(Rect(66,55,42,35,fillColor=colors.HexColor('#b7a584'),strokeColor=INK))
    label(d,39,70,'grip A',8,anchor='middle')
    label(d,87,70,'grip B',8,anchor='middle')
    label(d,63,38,'front grippers below flap',8,anchor='middle')
    d.add(Rect(120,82,235,70,fillColor=LIGHT,strokeColor=INK))
    label(d,237,139,'simple belt + three trays',10,anchor='middle')
    d.add(Line(134,96,340,96,strokeColor=INK,strokeWidth=2))
    d.add(Line(134,130,340,130,strokeColor=INK,strokeWidth=2))
    for x in [160,237,314]:
        d.add(Rect(x-29,93,58,40,fillColor=colors.HexColor('#d8e3e7'),strokeColor=INK))
        d.add(Circle(x,113,18,fillColor=colors.HexColor('#59656b'),strokeColor=INK))
    d.add(Rect(367,87,88,60,fillColor=BLUE,strokeColor=INK))
    label(d,411,125,'fixed rear',9,anchor='middle')
    label(d,411,110,'output flap',9,anchor='middle')
    label(d,411,95,'+ line sensor',8,anchor='middle')
    for x in [120,367]:
        d.add(Line(x-10,113,x,113,strokeColor=INK))
        d.add(Line(x,113,x-5,117,strokeColor=INK))
        d.add(Line(x,113,x-5,109,strokeColor=INK))
    label(d,18,8,'Beam phase: flap stays up. Disc phase: flap loads the front tray and the belt moves each disc to the rear.',8)
    return d

def control_diagram():
    d=Drawing(475,255)
    label(d,18,235,'Simple split control',12)
    label(d,120,216,'CareBot',11,anchor='middle')
    label(d,355,216,'SealBot',11,anchor='middle')
    d.add(Rect(66,174,108,30,fillColor=BLUE,strokeColor=INK))
    label(d,120,191,'ESP32-CAM',10,anchor='middle')
    label(d,120,180,'vision only',7,anchor='middle')
    d.add(Rect(66,126,108,36,fillColor=TEAL,strokeColor=INK,strokeWidth=1.2))
    label(d,120,146,'Uno R3',11,anchor='middle')
    label(d,120,133,'main control',7,anchor='middle')
    d.add(Line(120,174,120,162,strokeColor=INK))
    for x,a,b in [(12,'2 x DRV8833','3 DC motors'),(88,'PCA9685','9 servos'),(164,'TCS34725','colour + stops')]:
        d.add(Rect(x,66,68,40,fillColor=LIGHT,strokeColor=INK))
        label(d,x+34,89,a,8,anchor='middle')
        label(d,x+34,75,b,7,anchor='middle')
        d.add(Line(120,126,x+34,106,strokeColor=INK))
    d.add(Rect(301,147,108,42,fillColor=TEAL,strokeColor=INK,strokeWidth=1.2))
    label(d,355,171,'Uno R3',11,anchor='middle')
    label(d,355,157,'only control board',7,anchor='middle')
    for x,a,b in [(247,'2 x DRV8833','3 DC motors'),(323,'Direct','4 servos'),(399,'3 line outputs','+ index sensor')]:
        d.add(Rect(x,66,68,52,fillColor=LIGHT,strokeColor=INK))
        label(d,x+34,96,a,8,anchor='middle')
        label(d,x+34,80,b,7,anchor='middle')
        d.add(Line(355,147,x+34,118,strokeColor=INK))
    d.add(Rect(75,8,325,32,fillColor=colors.HexColor('#f1e1c7'),strokeColor=INK))
    label(d,237.5,25,'2S battery -> fuse + stop -> one logic buck + one actuator buck',9,anchor='middle')
    d.add(Line(120,66,120,40,strokeColor=INK,strokeWidth=.8))
    d.add(Line(355,66,355,40,strokeColor=INK,strokeWidth=.8))
    return d

def footer(canvas,doc):
    canvas.saveState()
    canvas.setStrokeColor(colors.HexColor('#ccd5da'))
    canvas.line(42,35,553,35)
    canvas.setFont('Helvetica',8)
    canvas.setFillColor(MUTED)
    canvas.drawString(42,23,'CareBot / SealBot | Simple Arduino architecture and purchasing plan | 7 September 2026')
    canvas.drawRightString(553,23,str(doc.page))
    canvas.restoreState()

story=[Spacer(1,30),p('CareBot and SealBot','TitleRobot'),p('Simple Arduino architecture and purchasing plan','SectionRobot'),Spacer(1,10),p('CareBot: Uno R3, PCA9685, and ESP32-CAM. SealBot: one Uno R3. Shared kit-and-patient lift with seven tilted boxes. Raised-flap beam grippers. JCB disc loader and indexed belt conveyor.'),Spacer(1,7),care_diagram(),Spacer(1,12),p('A complete mechanical, electrical, firmware, parts, pricing, dimensions, and prototype plan for the existing two-robot school project.'),p('Status: engineering proposal. Clearances, slopes, sensor thresholds and mission timing require tests with the real pieces and field.'),p('Complete new-build planning total: Rs 16,750, including shipping and repair reserve.','CaptionRobot'),PageBreak()]
lines=SOURCE.read_text(encoding='utf8').splitlines()
i=0
while i<len(lines):
    line=lines[i].strip()
    if not line or line.startswith('# '):
        i+=1; continue
    if line.startswith('## '):
        story.append(p(line[3:],'SectionRobot'))
        i+=1; continue
    if line.startswith('### '):
        story.append(p(line[4:],'SubRobot'))
        i+=1; continue
    if line.startswith('|'):
        rows=[]
        while i<len(lines) and lines[i].strip().startswith('|'):
            cells=[c.strip() for c in lines[i].strip().strip('|').split('|')]
            if not all(re.fullmatch(r'[-:]+',c) for c in cells): rows.append(cells)
            i+=1
        n=len(rows[0])
        assert all(len(r)==n for r in rows),rows
        if n==5 and rows[0][0]=='Part' and rows[0][2]=='Robocraze Rs': widths=[120,52,92,95,152]
        elif n==5 and rows[0][0] in {'Part','Material or service'}: widths=[125,55,55,70,206]
        elif n==5: widths=[73,70,116,145,107]
        elif n==4: widths=[85,112,172,142]
        elif n==3: widths=[115,210,186]
        else: widths=[162,349]
        content=[]
        for ri,row in enumerate(rows):
            content.append([Paragraph(('<b>'+inline(c)+'</b>') if ri==0 else inline(c),styles['CellRobot']) for c in row])
        table=Table(content,colWidths=widths,repeatRows=1,hAlign='LEFT')
        table.setStyle(TableStyle([('BACKGROUND',(0,0),(-1,0),LIGHT),('VALIGN',(0,0),(-1,-1),'TOP'),('LEFTPADDING',(0,0),(-1,-1),6),('RIGHTPADDING',(0,0),(-1,-1),6),('TOPPADDING',(0,0),(-1,-1),6),('BOTTOMPADDING',(0,0),(-1,-1),6),('LINEBELOW',(0,0),(-1,0),.8,INK),('LINEBELOW',(0,1),(-1,-1),.25,colors.HexColor('#d6dfe3'))]))
        story.extend([Spacer(1,5),KeepTogether([table]) if len(rows)<9 else table,Spacer(1,10)])
        if rows[0][0]=='Robot' and rows[0][1]=='Maximum build envelope':
            story.extend([start_diagram(),p('Figure 1. Proposed starting arrangement, dimensioned in start-local coordinates.','CaptionRobot')])
        continue
    if line.startswith('- '):
        story.append(p('- '+line[2:],'ListRobot'))
    elif re.match(r'^\d+\. ',line):
        story.append(p(line,'ListRobot'))
    else:
        story.append(p(line))
        if line.startswith('This gives the same action'):
            story.extend([sample_diagram(),p('Figure 2. The grippers are at the lower front, the movable input flap is directly above them, and the output flap stays fixed at the rear.','CaptionRobot')])
        if line.startswith('The ESP32-CAM remains CareBot'):
            story.extend([control_diagram(),p('Figure 3. CareBot adds one servo driver and one vision board; SealBot needs only its Uno R3.','CaptionRobot')])
    i+=1

doc=SimpleDocTemplate(str(OUTPUT),pagesize=(595.28,841.89),rightMargin=42,leftMargin=42,topMargin=38,bottomMargin=49,title='CareBot and SealBot: simple Arduino architecture and purchasing plan',author='Engineering proposal')
doc.build(story,onFirstPage=footer,onLaterPages=footer)
print(OUTPUT)
