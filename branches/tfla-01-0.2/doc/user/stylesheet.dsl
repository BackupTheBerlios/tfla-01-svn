<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY html-ss PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA dsssl>
<!ENTITY print-ss PUBLIC "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN" CDATA dsssl>]>
   
<!--
;;  This is the Stylesheet for my manual.
;;  Most of them is borrowed from the Linux Documentation Project.
;;  You can get their style sheet from http://metalab.unc.edu/gferg/ldp/ldp.dsl
;;  Read also their HOWTO (http://www.ibiblio.org/pub/Linux/docs/HOWTO/HOWTO-HOWTO)
;;
;;  http://www.miwie.org/docbook-dsssl-faq.html is also a good source for
;;  style settings
-->

<style-sheet>
	<style-specification id="print" use="print-stylesheet">
		<style-specification-body>
   
			
			(define %default-quadding% 'justify)
			
			;; customize the print stylesheet
			(define %paper-type% "A4")
			
			(define %generate-article-toc% #t)
			
			(define (toc-depth nd) 2)
			
			(define %hyphenation% #t)
			
			(define %section-autolabel%
			;; For enumerated sections (1.1, 1.1.1, 1.2, etc.)
  			#t)

			
			(define %body-start-indent%
  			;; Default indent of body text
  			0pi)

			(define %para-indent-firstpara%
  			;; First line start-indent for the first paragraph
  			0pt)

			(define %para-indent%
  			;; First line start-indent for paragraphs (other than the first)
  			0pt)

			(define %block-start-indent%
  			;; Extra start-indent for block-elements
  			0pt)
			
			(define %indent-programlisting-lines%
	        "    ")       (define %indent-screen-lines%
    		"    ")
			
			
			(define %generate-article-titlepage-on-separate-page%
  			;; Should the article title page be on a separate page?
  			#f)
			
			(define (article-titlepage-recto-elements)
			(list (normalize "title")
			(normalize "subtitle")
			(normalize "graphic")
			(normalize "corpauthor")
			(normalize "authorgroup")
			(normalize "author")
			(normalize "editor")
			(normalize "abstract")
			(normalize "legalnotice")
			(normalize "releaseinfo")))

            ;; Use PDF files for PDF creation
            (define %graphic-default-extension% "pdf")
			
		</style-specification-body>
	</style-specification>
	
	<style-specification id="html" use="html-stylesheet">
		<style-specification-body>
   
			(define %generate-article-toc% #t)
			
			(define (toc-depth nd) 3)
			
			(define %generate-part-toc% #t)

			(define %generate-article-titlepage% #t)

			(define (chunk-skip-first-element-list)
			;; forces the Table of Contents on separate page
			'())
			
			(define %section-autolabel%
			;; For enumerated sections (1.1, 1.1.1, 1.2, etc.)
  			#t)
			
			(define %html-ext% ".html")
			
			(define %use-id-as-filename%
			;; Use ID attributes as name for component HTML files?
  			#f)
			
			(define %root-filename%
  			;; The filename of the root HTML document (e.g, "index").
  			"muttprint")
			
		</style-specification-body>
	</style-specification>

	<external-specification id="print-stylesheet" document="print-ss">
	<external-specification id="html-stylesheet" document="html-ss">
</style-sheet>

<!-- vim:ts=4 sw=4
-->
