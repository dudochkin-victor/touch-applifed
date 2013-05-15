# This makefile works in the directories where the name of the main
# tex file is preceeds the other tex files in the lexical order.

MAINTEX=$(firstword $(sort $(basename $(wildcard *.tex))))

PDFVIEWER=xpdf
PSVIEWER=gv

EPSFILES=$(shell find -name '*.eps')

.PHONY: help
help:
	@echo ""
	@echo "make pdf   - creates $(MAINTEX).pdf"
	@echo "make ps    - creates $(MAINTEX).ps"
	@echo "make show  - creates $(MAINTEX).pdf and shows it with $(PDFVIEWER)"
	@echo "make showps- creates $(MAINTEX).ps and shows it with $(PSVIEWER)"
	@echo "make clean - cleans up directory"
	@echo "make help  - shows this help"
	@echo ""

.PHONY: pdf
pdf:	$(MAINTEX).pdf

.PHONY: ps
ps:	$(MAINTEX).ps

%.pdf: %.dvi
	dvipdf $(basename $<)

%.ps: %.dvi
	dvips $(basename $<)

%.dvi: %.tex $(wildcard *.tex) $(wildcard *.bib) $(EPSFILES)
	latex $(basename $<)
	bibtex $(basename $<) || true
	makeindex $(basename $<) || true
	( latex $(basename $<) | gawk '/Rerun/{rerun=1}{print $0}END{exit rerun}' ) || latex $(basename $^)

.PHONY: show
show:	$(MAINTEX).pdf
	$(PDFVIEWER) $^

.PHONY: showps
showps:	$(MAINTEX).ps
	$(PSVIEWER) $^

.PHONY: pub
pub:	$(MAINTEX).pdf
	$(PUB_COMMAND)

.PHONY: clean
clean:
	$(RM) $(addprefix $(MAINTEX), .dvi .pdf .ps .toc .bbl .blg .idx .ilg .ind) *~ */*~ *.aux *.log

