; $OpenXM: OpenXM/src/kxx/init-openxm.scm,v 1.5 2004/03/11 18:25:57 ohara Exp $

(define (openxm-plugin-eval t)
  (import-from (texmacs plugin plugin-cmd))
  (import-from (texmacs plugin plugin-convert))
  (plugin-eval "openxm" "default" (object->tree t)))

(define (openxm-plugin-eval-paste t)
  (insert-tree (object->tree (openxm-plugin-eval t))))

(define (openxm-path t)
  (if (string? t)
    (let* ((openxm-home (or (getenv "OpenXM_HOME") "/usr/local/OpenXM"))
		   (local-path (string-append openxm-home "/" t))
		   (web-prefix "http://www.math.kobe-u.ac.jp/OpenXM/Current/")
		   (web-path   (string-append web-prefix t)))
      (if (url-exists? local-path) local-path web-path))))

(define (w3m t)
  (if (string? t)
    (system (string-append "rxvt -g 100x50 -e w3m " t "&"))))

(define (w3m-search t)
  (w3m (openxm-path t)))

(define (openxm-initialize)
  (menu-extend texmacs-session-help-icons
    (if (in-openxm?)
    |
    ((balloon (icon "tm_help.xpm") "Risa/Asir manual")
     (w3m-search "doc/asir2000/html-jp/man_toc.html"))))
  (menu-extend texmacs-extra-menu
    (if (in-openxm?)
      (=> "OpenXM"
        (-> "Select engines"
          ("Risa/Asir" (insert-string "!asir;"))
          ("Kan/sm1"   (insert-string "!sm1;")))
        (-> "Output format"
          ("LaTeX"     (openxm-plugin-eval "!latex;"))
          ("verbatim"  (openxm-plugin-eval "!verbatim;")))
        ---
        (-> "Manuals"
          ("Risa/Asir manual"
           (w3m-search "doc/asir2000/html-jp/man_toc.html"))
          ("Asir-contrib manual"
           (w3m-search "doc/asir-contrib/html-ja/cman-ja_toc.html")))
        (-> "Web"
          ("The OpenXM Project"  (w3m "http://www.math.kobe-u.ac.jp/OpenXM/")))
        )))
)

(plugin-configure openxm
  (:require (url-exists-in-path? "openxm"))
  (:initialize (openxm-initialize))
  (:launch "exec openxm ox_texmacs")
  (:session "OpenXM[asir,sm1]"))
