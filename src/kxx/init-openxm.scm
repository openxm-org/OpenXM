; $OpenXM: OpenXM/src/kxx/init-openxm.scm,v 1.8 2004/03/16 04:53:29 ohara Exp $

(define (openxm-eval t)
  (import-from (texmacs plugin plugin-cmd))
  (import-from (texmacs plugin plugin-convert))
  (plugin-eval "openxm" "default" (object->tree t)))

(define (openxm-eval-paste t)
  (insert-tree (object->tree (openxm-eval t))))

(define (openxm-path t)
  (if (string? t)
    (let* ((openxm-home (or (getenv "OpenXM_HOME") "/usr/local/OpenXM"))
           (local-path (string-append openxm-home "/" t))
           (web-prefix "http://www.math.kobe-u.ac.jp/OpenXM/Current/")
           (web-path   (string-append web-prefix t)))
      (if (url-exists? local-path) local-path web-path))))

(define w3m-cmd
  (cond 
   ((url-exists-in-path? "w3m")
    (cond ((url-exists-in-path? "rxvt")  "rxvt  -g 100x50 -e w3m")
          ((url-exists-in-path? "xterm") "xterm -g 100x50 -e w3m")
          (else #f)))
   ((url-exists-in-path? "mozilla" ) "mozilla" )
   ((url-exists-in-path? "netscape") "netscape")
   ((url-exists-in-path? "iexplore") "iexplore")
   (else #f)))

(define (w3m t)
  (if (and (string? t) w3m-cmd)
    (system (string-append w3m-cmd " " t " &"))))

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
        (-> "Select display style"
          ("LaTeX"     (openxm-eval "!latex;"))
          ("verbatim"  (openxm-eval "!verbatim;")))
        (-> "Load Modules (Asir)"
          ("dsolv"     (openxm-eval "load(\"dsolv\");"))
          ("ccurve"    (openxm-eval "load(\"ccurve.rr\");"))
          ("yang"      (openxm-eval "load(\"yang.rr\");"))
          )
        (-> "Display Configuration (Asir)"
          ("Load default"   (openxm-eval "noro_print_env(0);"))
          ("Weyl algebra"   (openxm-eval "noro_print_env(\"weyl\");"))
          ("Euler OPs"      (openxm-eval "noro_print_env(\"yang\");"))
          )
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
