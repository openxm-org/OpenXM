; $OpenXM: OpenXM/src/texmacs/asir/progs/init-asir.scm,v 1.2 2004/02/28 19:00:48 ohara Exp $

(define (asir-initialize)
  (menu-extend help-menu
    (if (in-asir?) |
      ("Risa/Asir manual" (load-buffer "http://www.math.kobe-u.ac.jp/OpenXM/Current/doc/asir2000/html-en/man_toc.html")))
    (if (in-asir?) |
      ("Asir-contrib" (load-buffer "http://www.math.kobe-u.ac.jp/OpenXM/Current/doc/asir-contrib/html-en/cman-en_toc.html")))
    (if (in-asir?) |
      ("OpenXM Project" (load-buffer "http://www.math.kobe-u.ac.jp/OpenXM/")))
  ))

(plugin-configure asir
  (:require (url-exists-in-path? "ox_asir"))
;  (:initialize (asir-initialize))
  (:launch "tm_asir")
  (:session "Risa/Asir"))
