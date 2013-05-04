Latex style to be used to write dissertations in FEUP
=====================================================

a) Use "pdflatex", not "latex".

b) The main file is "mieic-en.tex". Read the instructions included in
the file.  For Portuguese use "mieic.tex".

c) The style is defined in "feupteses.sty".

d) The make the example file "mieic-en.pdf" use the Makefile or else:

   pdflatex mieic-en
   bibtex mieic-en
   pdflatex mieic-en
   pdflatex mieic-en

e) The file "plainnat.bst" is needed to process references in the
format (author,date).

f) Choose the character encoding used:


   \usepackage[latin1]{inputenc}

   OR

   \usepackage[utf8]{inputenc}

   OR for MAC native encoding

   \usepackage[applemac]{inputenc}


JCL & JCF, 2011-07-31
