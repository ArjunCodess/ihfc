from pathlib import Path
from PIL import Image, ImageDraw
from pypdf import PdfReader
root=Path('C:/Users/USER/Desktop/ihfc')
folder=root/'tmp/pdfs/full-design'
pages=sorted(folder.glob('review-*.png'))
for batch in range(0,len(pages),6):
    sheet=Image.new('RGB',(1200,1740),'#d5d9dc')
    d=ImageDraw.Draw(sheet)
    for j,p in enumerate(pages[batch:batch+6]):
        im=Image.open(p);im.thumbnail((580,820))
        x=(j%2)*600+10;y=(j//2)*580+25
        im.thumbnail((560,545))
        sheet.paste(im,(x,y));d.text((x,y-17),p.stem,fill='black')
    sheet.save(folder/f'contact-{batch//6+1}.png')
md=(root/'TWO-ROBOT-DESIGN-2026.md').read_text(encoding='utf8')
budget=md.split('## 10. Budget:')[1].split('The mechanism count')[0]
rows=[line.split('|') for line in budget.splitlines() if line.startswith('|')]
values=[int(r[-2].strip()) for r in rows if r[-2].strip().isdigit()]
assert sum(values[:-1])==values[-1]==9910,values
assert 50+70+50+80==250
reader=PdfReader(root/'tmp/pdfs/full-design-build/TWO-ROBOT-DESIGN-2026.pdf')
text='\n'.join(p.extract_text() for p in reader.pages)
assert '9,910' in text and 'Sample handling is excluded' not in text
print('Budget: Rs 9,910; score ceiling: 250; PDF pages:',len(reader.pages))
for n,p in enumerate(reader.pages,1):
    t=p.extract_text()
    print(n, len(t), t[:95].replace('\n',' '))
