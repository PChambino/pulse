#
# Makefile for feupteses/feupthesis
#
# Version:   Sat May 17 22:36:50 2008
# Written by JCL
#

## programs
LATEX=latex
PDFLATEX=pdflatex
BIBTEX=bibtex
DVIDVI=dvidvi -m '2:0(3mm,-14mm),1(193mm,-14mm)'
DVIPS=dvips -Ppdf
DVI2PS=dvips -t landscape -x 707
PS2PDF=ps2pdf -dEmbedAllFonts#true
MKIDX=makeindex

## Basename for result in PT
TARGET=mieic
## Basename for result in EN
#TARGET=mieic-en

## Basename for PDIS
#TARGET=pdis
#TARGET=pdis-en

## .tex files
TEXFILES=$(wildcard *.tex **/*.tex)

## BibTeX files
BIB=$(wildcard *.bib)

## paper
PAPERSIZE=a4

#we prefer pdflatex for bibtex
LATEXBIB=$(PDFLATEX)

main:	$(TARGET).pdf

index:
	$(MKIDX) $(TARGET)

biblio:
	$(BIBTEX) $(TARGET)

.tex.pdf:
	rm -f $(TARGET).pdf
	$(PDFLATEX) $< $@ || { rm -f $*.aux $*.idx && false ; }
	if grep 'There were undefined references' $*.log ; then \
	  $(BIBTEX) $(TARGET); $(PDFLATEX) $< $@; fi
	while grep 'Rerun to get cross-references right.' $*.log ; do \
	  $(PDFLATEX) $< $@ || { rm -f $*.aux $*.idx && false ; } ; done

.tex.dvi:
	rm -f $*.ps $*.pdf
	$(LATEX) $< || { rm -f $*.dvi $*.aux $*.idx && false ; }
	if grep 'There were undefined references' $*.log ; then \
	  $(BIBTEX) $(TARGET); $(LATEX) $< $@; fi
	while grep 'Rerun to get cross-references right.' $*.log ; do \
	  $(LATEX) $< || { rm -f $*.dvi $*.aux $*.idx && false ; } ; done

.dvi.ps:
	$(DVIPS) -t $(PAPERSIZE) $< -o $@

.dvi.dvi2:
	$(DVIDVI) $< $@

.dvi2.ps2:
	$(DVI2PS) $< -o $@

.ps.pdf:
	$(PS2PDF) $< $@

.bib.bbl:
# in case we don't already have a .aux file listing citations
	$(LATEXBIB) $(TARGET).tex
# get the citations out of the bibliography
	$(BIBTEX) $(TARGET)
# do it again in case there are out-of-order cross-references
	$(LATEXBIB) $(TARGET).tex
	$(BIBTEX) $(TARGET)

$(TARGET).pdf:	  $(TEXFILES)
$(TARGET).dvi:	  $(TEXFILES)
$(TARGET).ps:	  $(TARGET).dvi
$(TARGET).ps2:	  $(TARGET).dvi2
$(TARGET).dvi2:	  $(TARGET).dvi

## Extensions
EXTS=aux toc idx ind ilg log out lof lot lol bbl blg

##clean
clean:
	for EXT in ${EXTS}; do \
	  find `pwd` -name \*\.$${EXT} -exec rm -v \{\} \; ; done

##show PDF
display: $(TARGET).pdf
	acroread $(TARGET).pdf &

### misc
.SUFFIXES: .tex .aux .toc .lof .lot .log .dvi .dvi2 .ps .ps2 .pdf .bib .bbl
