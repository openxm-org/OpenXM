; $OpenXM: OpenXM/src/kxx/init-openxm.scm,v 1.1 2004/03/05 19:31:12 ohara Exp $

(define (w3m t)
  (if (string? t)
	(system (string-append "rxvt -g 100x50 -e w3m " t "&"))))

(define (w3m-local t)
  (if (string? t)
    (w3m (string-append "/usr/local/OpenXM/" t))))

(define (openxm-initialize)
  (menu-extend texmacs-session-help-icons
    (if (and (in-openxm?)
         (url-exists? "/usr/local/OpenXM/doc/asir2000/html-jp/man_toc.html"))
    |
    ((balloon (icon "tm_help.xpm") "Risa/Asir manual")
     (w3m-local "doc/asir2000/html-eg/man_toc.html"))))
  (menu-extend texmacs-extra-menu
    (if (in-openxm?)
      (=> "OpenXM"
		("Asir mode"    (insert-string "!asir;"))
		("Kan/sm1 mode" (insert-string "!sm1;"))
		---
		(-> "Manuals"
			("Risa/Asir manual"    (w3m-local "doc/asir2000/html-jp/man_toc.html"))
			("Asir-contrib manual" (w3m-local "doc/asir-contrib/html-ja/cman-ja_toc.html")))
		(-> "Web"
			("The OpenXM Project"  (w3m "http://www.math.kobe-u.ac.jp/OpenXM/")))
		)))
)

(plugin-configure openxm
  (:require (and (url-exists-in-path? "openxm") (url-exists-in-path? "ox_texmacs")))
  (:initialize (openxm-initialize))
  (:launch "openxm ox_texmacs")
  (:session "OpenXM"))
