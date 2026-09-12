"""Regenerate the LaTeX source from the authoritative Markdown design."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parent
BASE = ROOT / 'TWO-ROBOT-DESIGN-2026'

def escape(text):
    chars = {'\\': r'\textbackslash{}', '&': r'\&', '%': r'\%', '$': r'\$',
             '#': r'\#', '_': r'\_', '{': r'\{', '}': r'\}',
             '~': r'\textasciitilde{}', '^': r'\textasciicircum{}'}
    return ''.join(chars.get(c, c) for c in text)

def inline(text):
    result, pos = [], 0
    for m in re.finditer(r'\[([^]]+)\]\(([^)]+)\)', text):
        result.append(escape(text[pos:m.start()]))
        result.append(r'\href{' + m[2].replace('%', r'\%').replace('#', r'\#') + '}{' + escape(m[1]) + '}')
        pos = m.end()
    result.append(escape(text[pos:]))
    return ''.join(result)

preamble = r'''\documentclass[10pt,a4paper]{article}
\usepackage[margin=21mm,headheight=15pt]{geometry}
\usepackage[T1]{fontenc}
\usepackage{lmodern}
\usepackage{microtype}
\usepackage{longtable,array,booktabs}
\usepackage{enumitem}
\usepackage{xcolor}
\usepackage{fancyhdr}
\usepackage{needspace}
\usepackage{graphicx}
\usepackage[colorlinks=true,linkcolor=blue!45!black,urlcolor=blue!45!black]{hyperref}
\definecolor{ink}{HTML}{193F50}
\setlength{\parindent}{0pt}
\setlength{\parskip}{5pt}
\setlength{\emergencystretch}{2em}
\renewcommand{\arraystretch}{1.17}
\setlist{itemsep=4pt,topsep=4pt,leftmargin=1.7em}
\pagestyle{fancy}
\fancyhf{}
\fancyhead[L]{\small\color{ink}SealBot / CareBot}
\fancyhead[R]{\small Design revision 2026-09-06}
\fancyfoot[C]{\small\thepage}
\begin{document}
\hypersetup{pageanchor=false}
\begin{titlepage}
\color{ink}
\vspace*{20mm}
{\large ROBOTICS FOR GOOD 2026--2027\par}
\vspace{9mm}
{\Huge\bfseries Two robots. Nine days.\par}
\vspace{7mm}
{\Large SealBot and CareBot\par}
\vspace{12mm}
\rule{\linewidth}{1pt}
\vspace{8mm}
{\large Rs 9,910 planned / Rs 10,000 maximum\par}
{\large ESP32-CAM and simple mechanisms for a school team\par}
\vspace{12mm}
\textbf{SealBot}\par
Locate and collect three samples $\rightarrow$ fill all three laboratory slots
$\rightarrow$ place both containment beams $\rightarrow$ park clear.\par
\vspace{6mm}
\textbf{CareBot}\par
Collect twelve patients into numbered pockets $\rightarrow$ deliver by colour
$\rightarrow$ distribute all ten kits $\rightarrow$ finish with four green patients in RZ.\par
\vfill
\color{black}
Complete mission architecture and nine-day build plan\par
6 September 2026\par
Based on the supplied Rulebook-2026.pdf and official camera documentation.\par
Borrowed tools and practice surface assumed. Every scored task has a mechanism.\par
The 250-point ceiling is not a predicted score. Physical validation is required.\par
The supplied beam and boundary dimensions contain an unresolved inconsistency.
\end{titlepage}
\pagenumbering{roman}
\hypersetup{pageanchor=true}
\tableofcontents
\clearpage
\pagenumbering{arabic}
'''

lines = BASE.with_suffix('.md').read_text(encoding='utf-8-sig').splitlines()
out = [preamble]
i = 0
while i < len(lines):
    line = lines[i].strip()
    if not line or line.startswith('# ') or line.startswith('SealBot and CareBot |'):
        i += 1
        continue
    if line.startswith('## '):
        title = re.sub(r'^\d+\.\s*', '', line[3:])
        if title.startswith(('Budget:', 'Nine-day', 'Failure audit')):
            out.append('\\clearpage\n')
        out.append(r'\needspace{12\baselineskip}\section{' + inline(title) + '}\n')
        i += 1
    elif line.startswith('### '):
        reserve = 30 if line[4:].startswith('Pin allocation') else 10
        out.append(r'\needspace{' + str(reserve) + r'\baselineskip}\subsection{' + inline(line[4:]) + '}\n')
        i += 1
    elif line.startswith('!['):
        m = re.fullmatch(r'!\[([^]]+)\]\(([^)]+)\)', line)
        if not m:
            raise ValueError('Invalid image: ' + line)
        path = (ROOT / m[2]).resolve().as_posix()
        out.append(r'\begin{center}\includegraphics[width=\linewidth,height=0.40\textheight,keepaspectratio]{' + path + '}\n' + r'\par\small ' + inline(m[1]) + r'\end{center}' + '\n')
        i += 1
    elif line.startswith('|'):
        rows = []
        while i < len(lines) and lines[i].strip().startswith('|'):
            cells = [c.strip() for c in lines[i].strip().strip('|').split('|')]
            if not all(re.fullmatch(r'[-:]+', c) for c in cells):
                rows.append(cells)
            i += 1
        n = len(rows[0])
        if rows[0][0] == 'Robot':
            out.append(r'\needspace{19\baselineskip}' + '\n')
        widths = [0.34, 0.60] if n == 2 else ([0.17, 0.51, 0.23] if rows[0][0] == 'Robot' else [0.22, 0.42, 0.27])
        spec = '@{}' + ''.join(r'>{\raggedright\arraybackslash}p{' + str(w) + r'\linewidth}' for w in widths) + '@{}'
        header = ' & '.join(r'\textbf{' + inline(c) + '}' for c in rows[0]) + r' \\'
        out.append(r'\small\begin{longtable}{' + spec + '}\n' + r'\toprule' + '\n' + header + '\n' + r'\midrule\endfirsthead' + '\n' + r'\toprule' + header + r'\midrule\endhead' + '\n')
        for row in rows[1:]:
            out.append(' & '.join(inline(c) for c in row) + r' \\[3pt]' + '\n')
        out.append(r'\bottomrule\end{longtable}\normalsize' + '\n')
    elif line.startswith('- ') or re.match(r'^\d+\. ', line):
        numbered = not line.startswith('- ')
        env = 'enumerate' if numbered else 'itemize'
        pattern = r'^\d+\. ' if numbered else r'^- '
        out.append('\\begin{' + env + '}\n')
        while i < len(lines):
            s = lines[i].strip()
            if not s:
                i += 1
                continue
            if not re.match(pattern, s):
                break
            out.append(r'\item ' + inline(re.sub(pattern, '', s)) + '\n')
            i += 1
        out.append('\\end{' + env + '}\n')
    else:
        out.append(inline(line) + '\n\n')
        i += 1
out.append(r'\end{document}' + '\n')
BASE.with_suffix('.tex').write_text(''.join(out), encoding='utf-8')
print('Generated LaTeX from Markdown:', BASE.with_suffix('.tex'))
