; $OpenXM$

; (define (asir-initialize)
;   (menu-extend texmacs-session-help-icons
;     (if (and (in-asir?)
;          (url-exists? "$OpenXM_HOME/doc/asir2000/html-eg/man_toc.html"))
;     |
;     ((balloon (icon "tm_help.xpm") "Asir manual")
;      (load-help-buffer "$OpenXM_HOME/doc/asir2000/html-eg/man_toc.html")))
; 	))

(plugin-configure asir
  (:require (url-exists-in-path? "ox_asir"))
;  (:initialize (asir-initialize))
  (:launch "tm_asir")
  (:session "Asir"))
