; $OpenXM$

(define (openxm-initialize)
  (menu-extend texmacs-session-help-icons
    (if (and (in-openxm?)
         (url-exists? "/usr/local/OpenXM/doc/asir2000/html-eg/man_toc.html"))
    |
    ((balloon (icon "tm_help.xpm") "Risa/Asir manual")
     (load-help-buffer "/usr/local/OpenXM/doc/asir2000/html-eg/man_toc.html")))	)
  (menu-extend texmacs-extra-menu
    (if (in-openxm?)
      (=> "OpenXM"
		("Asir mode"    (insert-string "!asir;"))
		("Kan/sm1 mode" (insert-string "!sm1;"))
		---
		(-> "Manuals"
			("Risa/Asir manual" (load-help-buffer "/usr/local/OpenXM/doc/asir2000/html-eg/man_toc.html"))
			("Asir-contrib manual" (load-help-buffer "/usr/local/OpenXM/doc/asir-contrib/html-en/cman-en_toc.html")))
		(-> "Web"
			("The OpenXM Project" (load-buffer "http://www.math.kobe-u.ac.jp/OpenXM/")))
		)))
)

(plugin-configure openxm
  (:require (url-exists-in-path? "ox_texmacs"))
  (:initialize (openxm-initialize))
  (:launch "ox_texmacs")
  (:session "OpenXM"))
