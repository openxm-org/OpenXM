; $OpenXM: OpenXM/src/kxx/init-openxm.scm,v 1.3 2004/03/10 17:31:13 ohara Exp $

(define (my-openxm-plugin-eval t)
  (import-from (texmacs plugin plugin-cmd))
  (import-from (texmacs plugin plugin-convert))
  (plugin-eval "openxm" "default" (object->tree t)))

(define (my-openxm-plugin-eval-paste t)
  (insert-tree (object->tree (my-openxm-plugin-eval t))))

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
        (-> "Select engines"
          ("Risa/Asir" (insert-string "!asir;"))
          ("Kan/sm1"   (insert-string "!sm1;")))
        (-> "Output format"
          ("LaTeX"     (my-openxm-plugin-eval "!latex;"))
          ("verbatim"  (my-openxm-plugin-eval "!verbatim;")))
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
  (:launch "exec openxm ox_texmacs")
  (:session "OpenXM"))
